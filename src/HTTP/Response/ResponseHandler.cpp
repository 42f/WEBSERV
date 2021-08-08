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
}

/* ................................. METHODS .................................*/


ResponseHandler::result_type		ResponseHandler::processRequest() {

	if (_request.is_ok()) {
		Request req = _request.unwrap();



		// config::Server const& server = network::ServerPool::getServerMatch(getHeader(req, "Host"));




		fileHandler::File f("./assets/HTML_pages/index.html");
		f.getStream() >> _response;

		_response.setHeader(ResponseHeader(headerTitle::Content_Length, _response.getBodyLen()));
		_response.setHeader(ResponseHeader(headerTitle::Content_Type, "text/html; charset=UTF-8"));

		_response.setStatus(status::Ok);
		_result = result_type(_response);
	}
	else
		_result = result_type (Response(Version('4', '2'), status::BadRequest));

	_status = response_status::Ready;
	return _result;
}

// safely returns the value of a header if it exists, an empty string otherwise
std::string		ResponseHandler::getHeader(const Request & req, const std::string& target) {
	return req.get_header(target).unwrap_or("");
}

/* ................................. ACCESSOR ................................*/


/*
 * Returns the status of the response treatment, yet the
 * result itself has to be checked before unwraped.
 */
bool	ResponseHandler::isReady() {
	return (_status == response_status::Ready);
}

/*
 * Returns the result processed. If no call to processRequest was made prior
 * to a call to getResult, result sould not be unwrapped.
*/
ResponseHandler::result_type &	ResponseHandler::getResult() {
	return (_result);
}

/* ................................. OVERLOAD ................................*/


/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
