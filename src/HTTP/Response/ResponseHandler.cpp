#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

int RequestHandler::req_counter = 0;

ResponseHandler::ResponseHandler( void ) :
									_response(Version('4','2'), status::None),
									_result(_response)	{
}

/* ..............................COPY CONSTRUCTOR.............................*/

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler( void )	{
}

/* ................................. METHODS .................................*/

ResponseHandler::result_type		ResponseHandler::processRequest(Request const & req) {

	(void)req;

	_response.setStatus(status::Ok);

	std::stringstream io;

	io << "[request #" << RequestHandler::req_counter++ << "] hello , this is a response body. \nAnd a second line";

	io >> _response;
	return _result;
}


/* ................................. ACCESSOR ................................*/



/* ................................. OVERLOAD ................................*/


/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
