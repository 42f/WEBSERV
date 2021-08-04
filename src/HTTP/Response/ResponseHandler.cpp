#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

int RequestHandler::req_counter = 0;

// TODO: remove
std::vector<config::Server>  *ResponseHandler::_servers = NULL;

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

ResponseHandler::result_type		ResponseHandler::processRequest(Request const & req) {

	/*
	 *  HERE :
	 * - Get a match between request target and server is_match()
	 * 		- if no match, use first server from list
	 * 		- if match ?
	 *
	*/
	(void)req;

	_response.setStatus(status::Ok);

	std::stringstream io;

	io << "[request #" << RequestHandler::req_counter++ << "] hello , this is a response body. \nAnd a second line";

	io >> _response;
	return _result;
}


config::Server&				ResponseHandler::matchServer(Request const & req)	{

	(void)req;
	// TODO: get vector of config::Server from parser

	return (*_servers->begin());
}

/* ................................. ACCESSOR ................................*/



/* ................................. OVERLOAD ................................*/


/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
