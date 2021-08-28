//
// Created by alena on 30/07/2021.
//

#ifndef WEBSERV_REQUESTHANDLER_HPP
#define WEBSERV_REQUESTHANDLER_HPP

#include "HTTP/Request/Request.hpp"
#include "utils/Logger.hpp"

namespace request_status
{
	enum Status
	{
		Incomplete,		// Waiting for the request to make sense
		Waiting,		// Waiting on full body
		Complete,		// Ready to respond
		Error			// Error occured: Early close, Parse error
	};
}

/*
 * RequestHandler
 */
class RequestHandler
{
	friend class ResponseHandler;
public:
	typedef Result<Request, status::StatusCode>	result_type;

private:
	request_status::Status		_status;
	result_type 				_req;
	std::vector<char>			_buffer;
	int							_port;

	void			parse();
	bool			isPayloadAcceptable(Request const & r);
public:

	RequestHandler();

	result_type		update(const char *buff, size_t read, int port);
	result_type		receive();

	void			reset();

};

/* ************************************************************************** */

#endif //WEBSERV_REQUESTHANDLER_HPP
