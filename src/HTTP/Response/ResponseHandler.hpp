#pragma once

#include <vector>
#include <istream>
#include <iostream>
#include <sstream>
# include <unistd.h>
# include <iostream>
# include <string>

#include "Config/Server.hpp"
#include "Request/RequestLine.hpp"
#include "Headers/Headers.hpp"
#include "Config/Directives/Redirect.hpp"

#include "utils/Logger.hpp"
#include "HTTP/Request/Request.hpp"
#include "RequestHandler.hpp"
#include "Network/ServerPool.hpp"
#include "parser/Result.hpp"
#include "Status.hpp"
#include "Response.hpp"
#include "ResponseUtils/File.hpp"
#include "ResponseUtils/ResponseHeader.hpp"


namespace response_status
{
	enum Status
	{
		Empty,			// Not treated yet
		Waiting,		// Waiting on full body (large file or cgi)
		Ready		// Ready to be sent to client
	};
}

class ResponseHandler	{

		typedef	RequestHandler::result_type		ReqResult;
	public:

		void			init( ReqResult requestResult, int receivedPort );
		void	 		processRequest( void );
		Response & 		getResponse( void );
		bool		 	isReady( void );

		ResponseHandler( void );
		ResponseHandler( ReqResult requestResult, int receivedPort );
		~ResponseHandler( void );

	private:


		int		 							_port;
		ReqResult 							_request;
		response_status::Status				_status;
		Response							_response;
		// ! TODO remove, debug only
		static int							req_counter;

		config::Server&		matchServer(Request const & req);
		std::string			getHeader(const Request & req, const std::string& target);

		ResponseHandler( ResponseHandler const & src );
		ResponseHandler &		operator=( ResponseHandler const & rhs );
};

class BaseMethode	{



};

class GetMethode	: public BaseMethode {

};

class PostMethode	: public BaseMethode {

};

class DeleteMethode	: public BaseMethode {

};
