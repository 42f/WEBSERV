#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

ResponseHandler::ResponseHandler( ReqResult requestResult, int receivedPort ) : _method(NULL) {
	this->init(requestResult, receivedPort);
}

/* ..............................COPY CONSTRUCTOR.............................*/

ResponseHandler::ResponseHandler( void ) :
									_port(0),
									_request(ReqResult()),
									_method(NULL)	{
}

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler( void ) {
	if (_method != NULL)
		delete _method;
}

/* ................................. METHODS .................................*/

void	ResponseHandler::init( ReqResult const & requestResult, int receivedPort ) {

	_port = receivedPort;
	_method = NULL;
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

	if (_response.getState() != respState::emptyResp)
		return ;
	if (_request.is_ok()) {
		Request req = _request.unwrap();

		config::Server const& serverMatch = network::ServerPool::getServerMatch(getHeader(req, "Host"), _port);
		LocationConfig const locMatch = network::ServerPool::getLocationMatch(serverMatch, req.target);

		// Case where no location was resolved, and parent server has no root
		if (locMatch.get_root().empty())	{
			A_Method::makeErrorResponse(_response, status::Unauthorized, serverMatch);
			return ;
		}
		_method->handler(serverMatch, locMatch, req, _response);
	}
	else {
		A_Method::makeErrorResponse(_response, status::BadRequest, config::Server()); // waiting bugfix
		// A_Method::makeErrorResponse(_response, _request.unwrap_err(), config::Server()); // waiting bugfix
	}
}


// safely returns the value of a header if it exists, an empty string otherwise
std::string		ResponseHandler::getHeader(const Request & req, const std::string& target) {
	return req.get_header(target).unwrap_or("");
}



int	 		ResponseHandler::doSend( int fdDest, int flags)	{

	int state = _response.getState();

	if (state == respState::emptyResp) {
		// std::cout << "doSend -> emptyResp" << std::endl; // TODO cleanup
		return RESPONSE_IS_EMPTY;
	}
	if (state & respState::entirelySent) {
		// std::cout << "doSend -> entirelySent" << std::endl;
		return RESPONSE_SENT_ENTIRELY;
	}
	if (state & respState::readError) {
		// std::cout << "doSend -> readError" << std::endl;
		return -1;
	}
	if (state & respState::buffResp) {
		// std::cout << "doSend -> buffResp" << std::endl;
		return sendErrorBuffer(fdDest, flags);
	}
	if (state & respState::pipeResp) {
		// std::cout << "doSend -> pipeResp" << std::endl;
		return sendFromPipe(fdDest, flags);
	}
	if (state & respState::fileResp) {
		// std::cout << "doSend -> fileResp" << std::endl;
		return sendFromFile(fdDest, flags);
	}
	return -42; //TODO cleanup
}


	// std::cout << __func__ << ":" << __LINE__ << " MASK ============ " << _response.getState() << std::endl;

int			ResponseHandler::sendHeaders(int fdDest, int flags) {

	if ( (_response.getState() & respState::headerSent) == false) {
		std::stringstream output;
		output << _response;
		send(fdDest, output.str().c_str(), output.str().length(), flags);
		_response.getState() |= respState::headerSent;
		return (output.str().length());
	}
	return 0;
}



bool		ResponseHandler::isReady() {

	return _response.getState() &= (respState::fileResp
										| respState::pipeResp
										| respState::buffResp);
};



int			ResponseHandler::sendFromPipe(int fdDest, int flags) {
	if (sendHeaders(fdDest, flags) < 0)
		return (-1);
	return -1; // TODO implement
}



int			ResponseHandler::sendFromFile(int fdDest, int flags) {
	if (sendHeaders(fdDest, flags) < 0)
		return (-1);
	int retSend = doSendFromFD(_response.getFileInst().getFD(), fdDest, flags);
	switch ( retSend ) {
		case 0:
			_response.getState() = respState::entirelySent;
			break;
		case -1:
			_response.getState() = respState::readError;
			break;

		default:
			break;
	}
	return retSend;
}


int			ResponseHandler::sendErrorBuffer(int fdDest, int flags) {

	std::stringstream output;

	output << _response << _response.getErrorBuffer();
	send(fdDest, output.str().c_str(), output.str().length(), flags);
	_response.getState() = respState::entirelySent;
	return (output.str().length());
}



int			ResponseHandler::doSendFromFD(int fdSrc, int fdDest, int flags) {

	//TODO remove after Calixte code integration
	int set = 1;
	setsockopt(fdDest, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));

	char buff[DEFAULT_SEND_SIZE + 2];
	bzero(buff, DEFAULT_SEND_SIZE + 2);
	size_t	retRead = 0;

	if ( (retRead = read(fdSrc, buff, DEFAULT_SEND_SIZE)) < 0)
		return (RESPONSE_READ_ERROR);

	if (_response.getState() & respState::chunkedResp) {
		std::stringstream chunkData;
		chunkData << std::hex << retRead << "\r\n";
		send(fdDest, chunkData.str().c_str(), chunkData.str().length(), flags);

		buff[retRead + 0] = '\r';
		buff[retRead + 1] = '\n';
		send(fdDest, buff, retRead + 2 , flags);
	}
	else
		send(fdDest, buff, retRead, flags);
	return retRead;
}

/* ................................. ACCESSOR ................................*/


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
