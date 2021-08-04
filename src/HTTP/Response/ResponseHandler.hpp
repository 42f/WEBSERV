#pragma once

#include <vector>
#include <istream>
#include <iostream>
#include <sstream>
#include "../../Config/Server.hpp"
#include "../Request/RequestLine.hpp"
#include "../Headers/Headers.hpp"
#include "../Config/Directives/Redirect.hpp"

#include "../utils/Logger.hpp"
#include "../Request.hpp"
#include "../RequestHandler.hpp"
#include "../../parser/Result.hpp"
#include "../Status.hpp"
#include "Response.hpp"

# include <iostream>
# include <string>

namespace response_status
{
	enum Status
	{
		Empty,			// Not treated yet
		Waiting,		// Waiting on full body (large file or cgi)
		Complete		// Ready to be sent to client
	};
}

class ResponseHandler	{

	public:

		typedef Result<Response, status::StatusCode>	result_type;

		result_type 	processRequest(Request const & req);

		ResponseHandler( void );
		~ResponseHandler( void );
		// TODO: remove
		static std::vector<config::Server>  *_servers;

	private:

		config::Server&				matchServer(Request const & req);

		response_status::Status		_status;
		Response					_response;
		result_type					_result;


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
