#pragma once

#include <vector>
#include <istream>
#include <iostream>
#include <sstream>
#include "HTTP/Request/RequestLine.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "Config/Directives/Redirect.hpp"

#include "../utils/Logger.hpp"
#include "../Request.hpp"
#include "../RequestHandler.hpp"
#include "../Status.hpp"
#include "Response.hpp"

# include <iostream>
# include <string>

class ResponseHandler	{

	public:

		typedef Result<Response, status::StatusCode>	result_type;

		result_type 	processRequest(Request const & req);

		ResponseHandler( void );
		~ResponseHandler( void );

	private:

		request_status::Status		_status;
		Response					_response;
		result_type					_result;


		ResponseHandler( ResponseHandler const & src );
		ResponseHandler &		operator=( ResponseHandler const & rhs );
};
