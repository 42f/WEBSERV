#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

int	ResponseHandler::req_counter = 0;

ResponseHandler::ResponseHandler( RequestHandler::result_type & requestResult ) :
									_request(requestResult),
									_status(response_status::Empty),
									_result(result_type::err(status::None)) {
}

/* ..............................COPY CONSTRUCTOR.............................*/

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler( void )	{
	std::cout << RED << "_______________DESTRUCTOR_____________" << NC << std::endl;
}

/* ................................. METHODS .................................*/


ResponseHandler::result_type		ResponseHandler::processRequest() {

	/*
	 *  HERE :
	 * - Get a match between request target and server is_match()
	 * 		- if no match, use first server from list
	 * 		- if match ?
	 *
	*/

// !---- sleep
sleep(1);
	if (_request.is_ok()) {
		Request req = _request.unwrap();

		config::Server const& server = network::ServerPool::getMatch(getHeader(req, "Host"));

		std::stringstream io;

		io << "[request #" << ResponseHandler::req_counter++ << "] hello , this is a response body. \nAnd a second line\n";
		io << "The server used was: " << server.get_name();
		io >> _response;

		_response.setStatus(status::Ok);
		_result = result_type(_response);
	}
	else
		_result = result_type (Response(Version('4', '2'), status::BadRequest));

	_status = response_status::Complete;
	return _result;
}

// safely returns the value of a header if it exists, an empty string otherwise
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

bool	ResponseHandler::isReady() {
	return (_status == response_status::Complete);
}

ResponseHandler::result_type &	ResponseHandler::getResult() {
	return (_result);
}

/* ................................. OVERLOAD ................................*/


/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
