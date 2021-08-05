#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

int	ResponseHandler::req_counter = 0;

ResponseHandler::ResponseHandler( void ) :
									_status(response_status::Empty),
									_response(Version('4','2'), status::None),
									_result(result_type::err(status::None)) {
}

/* ..............................COPY CONSTRUCTOR.............................*/

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler( void )	{
}

/* ................................. METHODS .................................*/


ResponseHandler::result_type		ResponseHandler::processRequest(RequestHandler::result_type const & res) {

	/*
	 *  HERE :
	 * - Get a match between request target and server is_match()
	 * 		- if no match, use first server from list
	 * 		- if match ?
	 *
	*/

	if (res.is_ok()) {
		Request req = res.unwrap();
		_response.setStatus(status::Ok);

		config::Server const& server = network::ServerPool::getMatch(getHeader(req, "Host"));

		std::stringstream io;
		io << "[request #" << ResponseHandler::req_counter++ << "] hello , this is a response body. \nAnd a second line\n";
		io << "The server used was: " << server.get_name();
		io >> _response;
		_result = result_type(_response);
		return _result;
	}
	else
		return (Response(Version('9', '8'), status::BadRequest));		// TODO error management
}

std::string		ResponseHandler::getHeader(const Request & req, const std::string& target) {

	Result<std::string>	result = req.get_header(target);

	if (result.is_ok())	{
		return result.unwrap();
	}
	else {
		return std::string("");
	}
}

/* ................................. ACCESSOR ................................*/

/* ................................. OVERLOAD ................................*/


/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
