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

Result<std::string>		Request::get_header(const std::string &name) const
{
	std::map<std::string, Header>::const_iterator it = this->_headers.find(name);
	if (it == this->_headers.end())
		return Result<std::string>::err(DefaultError("Unknown cgi"));
	return Result<std::string>::ok(it->second.value());
}

bool Request::receive(std::vector<char> &vector)
{
	(void)vector;
	return false;
}

std::ostream &operator<<(std::ostream & stream, const Request &req)
{
	stream << req.method << " " << req.target << " HTTP/" << req.version;
	return stream;
}
