//
// Created by alena on 14/07/2021.
//

#include "Request.hpp"

/*
 * Request
 */
Request::Request() { };

Request::Request(methods::s_method method, Target target, Version version): method(method), target(target), version(version) { }

void	Request::set_header(const Header& header)
{
	this->_headers.insert(std::make_pair(header.name(), header));
}

Result<std::string>		Request::get_header(const std::string &name)
{
	std::map<std::string, Header>::iterator it = this->_headers.find(name);
	if (it == this->_headers.end())
		return Result<std::string>::err(DefaultError("Unknown cgi"));
	return Result<std::string>::ok(it->second.value());
}

std::ostream &operator<<(std::ostream & stream, const Request &req)
{
	stream << req.method << " " << req.target << " HTTP/" << req.version;
	return stream;
}

bool Request::receive(std::vector<char> &vector) {
	(void)vector;
	return false;
}

/*
 * RequestParser
 */
RequestParser::RequestParser() { }

RequestParser::result_type	RequestParser::operator()(const slice& input)
{
	ParserResult<tuple<methods::s_method, Target, Version> > line = RequestLine()(input);
	Request		req;

	if (line.is_ok())
	{
		tuple<methods::s_method, Target, Version>	info = line.unwrap();
		req = Request(info.first, info.second, info.third);
	}
	else {
		return result_type::err(line.left(), error("Failed to parse request line"));
	}
	ParserResult<std::vector<Header> >	res =
	terminated(many(terminated(Headers(), newline)), Newline())(line.left());
	std::cout << "****Parsing****" << std::endl << req << std::endl;
	if (res.is_ok())
	{
		std::vector<Header>	&v = res.unwrap();
		for (std::vector<Header>::iterator it = v.begin(); it != v.end(); it++)
		{
			req.set_header(*it);
			std::cout << *it << std::endl;
		}
	}
	else
		std::cerr << res.unwrap_err() << std::endl;
	return res.map(req);
	// TODO if this is not here, either the request is incomplete or ill-formatted
}


/*
 * RequestHandler
 */
RequestHandler::RequestHandler() : _transfer_type(transfer::Unset), _status(status::Incomplete), _req(Result<Request>::err(DefaultError(""))) {
	_buffer.reserve(512);
}

// if client timeout, there's no need to call this

Result<Request> RequestHandler::receive() {
	if (_req.unwrap().receive(_buffer)) {
		_status = status::Complete;
		return _req;
	}
	return Result<Request>::err(DefaultError("waiting"));
}

Result<Request>		RequestHandler::update(const char *buff, size_t read)
{
	if (_status == status::Complete || _status == status::Error)
		return _req;
	//TODO check content length / transfer encoding and store it
	//TODO handle zero size transfer (connection closed) if chunked transfer. else error
	size_t offset = _buffer.size();
	_buffer.reserve(offset + read);
	std::vector<char>::iterator off = _buffer.begin() + offset;
	_buffer.insert(off, buff, buff + read);
	switch (_status) {
		case status::Incomplete:
			if (parse() == status::Complete)
				return _req;
			return receive();
		case status::Waiting:
			 return receive();
		default:
			break;
	}
	return Result<Request>::err(DefaultError("unreachable"));
}

void		RequestHandler::reset()
{
	// TODO if the request is complete, keep the rest of the body instead of trashing it
	_buffer.clear();
	_transfer_type = transfer::Unset;
	_status = status::Incomplete;
	_req = Result<Request>::err(DefaultError(""));
}

/*
 * Private fonction
 */
status::Status		RequestHandler::parse()
{
	ParserResult<Request>	req = RequestParser()(slice(_buffer.data(), _buffer.size()));
	if (req.is_ok())
	{
		Request	r = req.unwrap();

		_req = Result<Request>::ok(r);
		//std::cout << "parsed: " << (req.left().p - _buffer.data()) << std::endl;
		std::vector<char>::iterator		end = _buffer.begin() + (req.left().p - _buffer.data());

		_buffer.erase(_buffer.begin(), end);
		std::cout << "left: [" << slice(_buffer.data(), req.left().size) << "]" << std::endl;
		// TODO on Request: add method to check presence of body
		_status = status::Complete;
	}
	return _status;
}
