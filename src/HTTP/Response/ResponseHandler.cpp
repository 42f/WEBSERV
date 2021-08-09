#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

int	ResponseHandler::req_counter = 0;

ResponseHandler::ResponseHandler( ReqResult requestResult ) :
										_request(requestResult),
										_status(response_status::Empty) {
}

/* ..............................COPY CONSTRUCTOR.............................*/

ResponseHandler::ResponseHandler( void ) :
									_request(ReqResult()),
									_status(response_status::Empty)	{
}

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler( void ) {}

/* ................................. METHODS .................................*/

void	ResponseHandler::init( ReqResult requestResult ) {
	_request = requestResult;
}

void	ResponseHandler::processRequest() {

	if (_request.is_ok()) {
		Request req = _request.unwrap();



		// config::Server const& server = network::ServerPool::getServerMatch(getHeader(req, "Host"));




		files::File f("./assets/HTML_pages/index.html");
		f.getStream() >> _response;

		_response.setHeader(ResponseHeader(headerTitle::Content_Length, _response.getBodyLen()));
		_response.setHeader(ResponseHeader(headerTitle::Content_Type, "text/html; charset=UTF-8"));

		_response.setStatus(status::Ok);
	}
	else
		_response = Response(Version('4', '2'), status::BadRequest);	// TODO change 4, 2. debugonly

	_status = response_status::Ready;
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
Response &	ResponseHandler::getResponse() {
	return (_response);
}

/* ................................. OVERLOAD ................................*/


/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
