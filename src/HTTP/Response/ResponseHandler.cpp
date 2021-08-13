#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

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

void	ResponseHandler::init( ReqResult const & requestResult, int receivedPort ) {

	_status = response_status::Empty;
	_port = receivedPort;
	_request = requestResult;
	if (_request.is_ok())	{
		switch (_request.unwrap().method)
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
		std::cout << RED << "LOC MATCHED: " << std::endl;
		std::cout << locMatch << std::endl;

		_method->handler(serverMatch, locMatch, req, _response);
		getBodyPart(42);
	}
	else {
		_response = Response(Version('D', 'B'), _request.unwrap_err());	// TODO change version debugonly
	}
	_status = response_status::Ready;
}

// safely returns the value of a header if it exists, an empty string otherwise
std::string		ResponseHandler::getHeader(const Request & req, const std::string& target) {
	return req.get_header(target).unwrap_or("");
}

std::string const &	ResponseHandler::getBodyPart( size_t nBytes ) {

	Timer	time;	// TODO remove debug
	std::ifstream & file = _response.getFile().getStream();
    char c;
	_bodyBuffer.clear(); // TODO useful ?...

	time.start();
    file.get(c);
    for (size_t i = 0; file.good() && i < nBytes - 1; i++) {
        _bodyBuffer.push_back(c);
        file.get(c);
    }
	LogStream s; s << "time to read file: " << time.getTimeElapsed() << "ms. Body size: " << _bodyBuffer.size();	// TODO remove debug
	return _bodyBuffer;
}
/* ................................. ACCESSOR ................................*/


bool	ResponseHandler::isHeadReady() {
	return (_status == response_status::Ready);
}

bool	ResponseHandler::isBodyReadable() {
	return (_status == response_status::Ready && _response.getFile().isGood());
}

/*
 * Returns the result processed. If no call to processRequest was made prior
 * to a call to getResult, result sould not be unwrapped.
*/
Response const &	ResponseHandler::getResponse() {
	return (_response);
}

/* ................................. OVERLOAD ................................*/


/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
