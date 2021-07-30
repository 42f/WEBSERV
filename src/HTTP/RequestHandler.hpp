//
// Created by alena on 30/07/2021.
//

#ifndef WEBSERV_REQUESTHANDLER_HPP
#define WEBSERV_REQUESTHANDLER_HPP

#include "Request.hpp"

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

namespace transfer
{
	enum TransferType
	{
		Unset,			// None yet
		Identity,		// Content-Length
		Chunked			// Transfer-Encoding
	};
}

/*
 * RequestHandler
 */
class RequestHandler
{
public:
	typedef Result<Request, status::StatusCode>	result_type;

private:
	transfer::TransferType		_transfer_type;
	request_status::Status		_status;
	result_type 				_req;
	std::vector<char>			_buffer;

	void			parse();
public:
	RequestHandler();

	result_type 	update(const char *buff, size_t read);
	result_type 	receive();

	void			reset();
};


#endif //WEBSERV_REQUESTHANDLER_HPP
