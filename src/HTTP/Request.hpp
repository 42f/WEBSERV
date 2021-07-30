//
// Created by alena on 14/06/2021.
//

#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include "HTTP/Request/RequestLine.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "Config/Directives/Redirect.hpp"

#include <map>

/*
 * Request
 */
struct Request
{
private:
	std::map<std::string, Header>			_headers;
	std::vector<char>						_body;

public:
	methods::s_method						method;
	Target									target;
	Version									version;

	Request();

	Request(methods::s_method method, Target target, Version version);

	void				set_header(const Header& header);
	Result<std::string>	get_header(const std::string& name) const;
	std::vector<char>	body();

	//TODO check end of body

	bool receive(std::vector<char> &vector);

	friend std::ostream &operator<<(std::ostream & stream, const Request &req);
};

#endif //WEBSERV_REQUEST_HPP
