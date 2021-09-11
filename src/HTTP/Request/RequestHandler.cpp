//
// Created by alena on 30/07/2021.
//

#include "RequestHandler.hpp"
#include "HTTP/Request/RequestParser.hpp"
#include "Network/ServerPool.hpp"
#include <cstdlib>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
RequestHandler::RequestHandler() : _status(request_status::Incomplete), _req(result_type::err(status::None))
{
	_buffer.reserve(512);
}

/*
** --------------------------------- METHODS ----------------------------------
*/
// if client timeout, there's no need to call this
RequestHandler::result_type RequestHandler::receive()
{
	Result<bool>	res = _req.unwrap().receive(_buffer);
	if (res.is_err()) {
		std::cout << "is this an error ?" << std::endl;
		_status = request_status::Error;
		return _req;
	} else if (res.unwrap()) {
		_status = request_status::Complete;
		return _req;
	}
	return result_type::err(status::Incomplete);
}

RequestHandler::result_type RequestHandler::update(const char *buff, size_t read, int port)
{
	_port = port;
	if (_status == request_status::Complete || _status == request_status::Error)
		return _req;
	size_t offset = _buffer.size();

	_buffer.reserve(offset + read);
	std::vector<char>::iterator off = _buffer.begin() + offset;
	_buffer.insert(off, buff, buff + read);
	if (_status == request_status::Incomplete)
		parse();
	if (_status == request_status::Waiting)
	{
		RequestHandler::result_type t = receive();
		return t;
	}
	return _req;
}

void		RequestHandler::reset()
{
	//TODO if the request is complete, keep the rest of the body instead of trashing it
	_buffer.clear();
	_status = request_status::Incomplete;
	_req = result_type::err(status::None);
}

/*
** --------------------------------- PRIVATE ----------------------------------
*/
void	RequestHandler::parse()
{
	slice					input = slice(_buffer.data(), _buffer.size());
	ParserResult<Request>	req = RequestParser()(input);

	if (req.is_err())
	{
		{
			LogStream stream;
			stream << req.unwrap_err();
 #if LOG_LEVEL == LOG_LEVEL_TRACE
 			req.unwrap_err().trace(input, stream); // TODO remove ? segv with chunk req
 #endif
		}
	}
	if (req.is_ok())
	{
		Request	r = req.unwrap();

		_req = result_type::ok(r);
		std::vector<char>::iterator		end = _buffer.begin() + (req.left().p - _buffer.data());
		_buffer.erase(_buffer.begin(), end);
		if (r.get_header("Content-Length").is_ok() || r.get_header("Transfer-Encoding").is_ok()) {
			if (r.get_header("Content-Length").is_ok()) {
				if (isPayloadAcceptable(r) == false) {
					_status = request_status::Error;
					_req = result_type::err(status::PayloadTooLarge);
					return;
				}
				_buffer.reserve(std::atoi(r.get_header("Content-Length").unwrap().c_str()));
			}
			else {
				_buffer.reserve(65550);
			}
			_status = request_status::Waiting;
		}
		else
			_status = request_status::Complete;
	}
	else
	{
		status::StatusCode code = req.unwrap_err().content();
		_status = code == status::Incomplete ? request_status::Incomplete : request_status::Error;
		_req = result_type::err(code == status::None ? status::BadRequest : code);
	}
}

bool RequestHandler::isPayloadAcceptable(Request const &r) {

  if (r.get_header("Host").is_ok() && r.get_header("Content-Length").is_ok()) {
    config::Server serv = network::ServerPool::getServerMatch(
        r.get_header("Host").unwrap(), _port);
    LocationConfig loc =
        network::ServerPool::getLocationMatch(serv, r.target);
	size_t reqLen = std::atoi(r.get_header("Content-Length").unwrap().c_str());
	return reqLen < loc.get_body_size();
  }
  return true;
}

/* ************************************************************************** */
