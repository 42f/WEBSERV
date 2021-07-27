//
// Created by alena on 14/06/2021.
//

#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include "parser/export.hpp"
#include "Tokens.hpp"

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
	Result<std::string>	get_header(const std::string& name);
	std::vector<char>	body();

	//TODO check end of body


	friend std::ostream &operator<<(std::ostream & stream, const Request &req);

	bool receive(std::vector<char> &vector);
};

/*
 * RequestParser
 */
class RequestParser: public Parser<Request>
{
public:
	RequestParser();

	result_type operator()(const slice& input);
};

/*
 * RequestHandler
 */
namespace status
{
	enum Status
			{
		Incomplete,		// Waiting for the request to make sense
		Waiting,		// Waiting on full body
		Complete,		// Ready to respond
		Error			// Error occured: Early close, Parse error
	};
}

namespace transfer
{
	enum TransferType
			{
		Unset,			// None yet
		Identity,		// Content-Length
		Chunked			// Transfer-Encoding
	};
}

class RequestHandler
{
private:
	transfer::TransferType	_transfer_type;
	status::Status			_status;
	Result<Request>			_req;
	std::vector<char>		_buffer;

	status::Status		parse();
public:
	RequestHandler();

	Result<Request>	update(const char *buff, size_t read);
	Result<Request> receive();

	void			reset();
};


#endif //WEBSERV_REQUEST_HPP
