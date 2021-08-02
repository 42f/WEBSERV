//
// Created by alena on 30/07/2021.
//

#include "RequestHandler.hpp"
#include "RequestParser.hpp"

/*
 * RequestHandler
 */
RequestHandler::RequestHandler() : _status(request_status::Incomplete), _req(result_type::err(status::None))
{
	_buffer.reserve(512);
}

// if client timeout, there's no need to call this
RequestHandler::result_type RequestHandler::receive()
{
	if (_req.unwrap().receive(_buffer))
	{
		_status = request_status::Complete;
		std::cout << "body: [" << slice(_req.unwrap().get_body().data(), _req.unwrap().get_body().size()) << "]" << std::endl;
		std::cout << "left: [" << slice(_buffer.data(), _buffer.size()) << "]" << std::endl;
		return _req;
	}
	return result_type::err(status::None);
}

RequestHandler::result_type RequestHandler::update(const char *buff, size_t read)
{
	if (_status == request_status::Complete || _status == request_status::Error)
		return _req;
	size_t offset = _buffer.size();
	_buffer.reserve(offset + read);
	std::vector<char>::iterator off = _buffer.begin() + offset;
	_buffer.insert(off, buff, buff + read);
	switch (_status)
	{
		case request_status::Incomplete:
			parse();
			if (_status == request_status::Error || _status == request_status::Complete)
				return _req;
			return receive();
		case request_status::Waiting:
			return receive();
		default:
			break;
	}
	return result_type::err(status::None);
}

void		RequestHandler::reset()
{
	//TODO if the request is complete, keep the rest of the body instead of trashing it
	_buffer.clear();
	_status = request_status::Incomplete;
	_req = result_type::err(status::None);
}

/*
 * Private fonction
 */
void	RequestHandler::parse()
{
	slice					input = slice(_buffer.data(), _buffer.size());
	ParserResult<Request>	req = RequestParser()(input);

	if (req.is_err())
		req.unwrap_err().trace(input);
	if (req.is_ok())
	{
		Request	r = req.unwrap();

		_req = result_type::ok(r);
		std::vector<char>::iterator		end = _buffer.begin() + (req.left().p - _buffer.data());
		_buffer.erase(_buffer.begin(), end);
		if (r.get_header("Content-Length").is_ok() || r.get_header("Transfer-Encoding").is_ok())
			_status = request_status::Waiting;
		else
			_status = request_status::Complete;
	}
	else if (req.unwrap_err().content() != status::None)
	{
		_status = request_status::Error;
		_req = result_type::err(req.unwrap_err().content());
	}
}
