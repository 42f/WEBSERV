#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

int 							ResponseHandler::req_counter = 0;
std::vector<config::Server>		ResponseHandler::_servers = std::vector<config::Server>();

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


void	ResponseHandler::init(std::string const &configFilePath) {
	ResponseHandler::_servers = config::parse(configFilePath);
}

ResponseHandler::result_type		ResponseHandler::processRequest(RequestHandler::result_type const & req) {

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

	io << "[request #" << ResponseHandler::req_counter++ << "] hello , this is a response body. \nAnd a second line";

	io >> _response;
	_result = result_type(_response);
	return _result;
}


config::Server&				ResponseHandler::matchServer(Request const & req)	{

	(void)req;
	// TODO: get vector of config::Server from parser

	return (*_servers.begin());
}

/* ................................. ACCESSOR ................................*/

std::vector<config::Server> const &	ResponseHandler::getServers( void ) {

	return _servers;
}

/* ................................. OVERLOAD ................................*/


/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
