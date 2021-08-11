#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

int	ResponseHandler::req_counter = 0;

ResponseHandler::ResponseHandler( ReqResult requestResult, int receivedPort ) {
	this->init(requestResult, receivedPort);
}

/* ..............................COPY CONSTRUCTOR.............................*/

ResponseHandler::ResponseHandler( void ) :
									_port(0),
									_request(ReqResult()),
									_status(response_status::Empty),
									_method(NULL)	{
}

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler( void ) {
	if (_method != NULL)
		delete _method;
}

/* ................................. METHODS .................................*/

void	ResponseHandler::init( ReqResult requestResult, int receivedPort ) {

	_port = receivedPort;
	_request = requestResult;
	_status = response_status::Empty;
	if (requestResult.is_ok())	{
		switch (requestResult.unwrap().method)
		{
			case methods::GET :
				_method = new GetMethod;
				break;
			case methods::POST :
				_method = new PostMethod;
				break;
			case methods::DELETE :
				_method = new DeleteMethod;
				break;

			default:
				_method = new UnsupportedMethod;
				break;
		}
	}
}

void	ResponseHandler::processRequest() {

	if (_request.is_ok()) {
		Request req = _request.unwrap();

		config::Server const& serverMatch = network::ServerPool::getServerMatch(getHeader(req, "Host"), _port);
		LocationConfig const locMatch = network::ServerPool::getLocationMatch(serverMatch, req.target);

		_method->handler(serverMatch, locMatch, req, _response);


	}
	else {
		_response = Response(Version('4', '2'), _request.unwrap_err());	// TODO change 4, 2. debugonly
	}
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
