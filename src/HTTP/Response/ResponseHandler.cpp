#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

ResponseHandler::ResponseHandler(ReqResult requestResult, int receivedPort)
    : _method(NULL) {
    this->init(requestResult, receivedPort);
}

/* ..............................COPY CONSTRUCTOR.............................*/

ResponseHandler::ResponseHandler(void)
    : _port(0), _request(ReqResult()), _method(NULL) {}

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler(void) {
    if (_method != NULL) delete _method;
}

/* ................................. METHODS .................................*/

void ResponseHandler::init(ReqResult const& requestResult, int receivedPort) {
    _port = receivedPort;
    _method = NULL;
    _request = requestResult;
    if (_request.is_ok()) {
        switch (_request.unwrap().method) {
            case methods::GET:
                _method = new GetMethod;
                break;
            case methods::POST:
                _method = new PostMethod;
                break;
            case methods::DELETE:
                _method = new DeleteMethod;
                break;

            default:
                _method = new UnsupportedMethod;
                break;
        }
    }
}

void ResponseHandler::processRequest() {
    if (_response.getState() != respState::emptyResp) return;
    if (_request.is_ok()) {
        Request req = _request.unwrap();

        config::Server const& serverMatch =
            network::ServerPool::getServerMatch(getHeader(req, "Host"), _port);
        LocationConfig const locMatch =
            network::ServerPool::getLocationMatch(serverMatch, req.target);

        // Case where no location was resolved, and parent server has no root
        if (locMatch.get_root().empty()) {
            A_Method::makeErrorResponse(_response, status::Unauthorized,
                                        serverMatch);
            return;
        }
        _method->handler(serverMatch, locMatch, req, _response);
    } else {
        A_Method::makeErrorResponse(_response, status::BadRequest,
                                    config::Server());  // TODO waiting bugfix
        // A_Method::makeErrorResponse(_response, _request.unwrap_err(),
        // config::Server()); // waiting bugfix
    }
}

// safely returns the value of a header if it exists, an empty string otherwise
std::string ResponseHandler::getHeader(const Request& req,
                                       const std::string& target) {
    return req.get_header(target).unwrap_or("");
}

int ResponseHandler::doSend(int fdDest, int flags) {
#if __APPLE__
    int set = 1;
    setsockopt(fdDest, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif

    int state = _response.getState();

    if (state == respState::emptyResp) {
        return RESPONSE_IS_EMPTY;
    }
    if (state & respState::entirelySent) {
        return RESPONSE_SENT_ENTIRELY;
    }
    if (state & respState::readError) {
        return -1;
    }
    if (state & respState::buffResp) {
        return sendErrorBuffer(fdDest, flags);
    }
    if (state & respState::pipeResp) {
        return sendFromPipe(fdDest, flags);
    }
    if (state & respState::fileResp) {
        return sendFromFile(fdDest, flags);
    }
    if (state & respState::noBodyResp) {
        return sendHeaders(fdDest, flags);
    }
    return -42;  // TODO cleanup
}

int ResponseHandler::sendHeaders(int fdDest, int flags) {
    if ((_response.getState() & respState::headerSent) == false) {
        std::stringstream output;
        output << _response;
        send(fdDest, output.str().c_str(), output.str().length(), flags);
        _response.getState() |= respState::headerSent;
        if (_response.getState() & respState::noBodyResp) {
            _response.getState() |= respState::entirelySent;
            return (RESPONSE_SENT_ENTIRELY);
        }
        return (output.str().length());
    }
    return 0;
}

bool ResponseHandler::isReady() {
    return _response.getState() &=
           (respState::fileResp | respState::pipeResp | respState::buffResp);
};

int ResponseHandler::sendFromPipe(int fdDest, int flags) {
    // TODO implement
    sendHeaders(fdDest, flags);
    sendFromFile(fdDest, flags);
    return 1;
}

int ResponseHandler::sendFromFile(int fdDest, int flags) {
    sendHeaders(fdDest, flags);
    int retSend = doSendFromFD(_response.getFileInst().getFD(), fdDest, flags);
    switch (retSend) {
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

int ResponseHandler::sendErrorBuffer(int fdDest, int flags) {
    std::stringstream output;

    output << _response << _response.getErrorBuffer();
    send(fdDest, output.str().c_str(), output.str().length(), flags);
    _response.getState() = respState::entirelySent;
    return RESPONSE_SENT_ENTIRELY;
}

int ResponseHandler::doSendFromFD(int fdSrc, int fdDest, int flags) {
    char buff[DEFAULT_SEND_SIZE + 2];
    bzero(buff, DEFAULT_SEND_SIZE + 2);
    ssize_t retRead = 0;

    if ((retRead = read(fdSrc, buff, DEFAULT_SEND_SIZE)) < 0) {
        _response.getState() = respState::readError;
        return (RESPONSE_READ_ERROR);
    }

    if (_response.getState() & respState::chunkedResp) {


        std::stringstream chunkSize;
        chunkSize << std::hex << retRead << "\r\n";
        std::string chunkData(chunkSize.str());
        chunkData.reserve(chunkData.length() + DEFAULT_SEND_SIZE + 2);
        buff[retRead + 0] = '\r';
        buff[retRead + 1] = '\n';
        chunkData.insert(chunkData.end(), buff, buff + retRead + 2);

        send(fdDest, chunkData.data(), chunkData.length(), flags);
        return retRead;
    } else {
        send(fdDest, buff, retRead, flags);
        return RESPONSE_SENT_ENTIRELY;
    }
}

/* ................................. ACCESSOR ................................*/

/*
 * Returns the result processed. If no call to processRequest was made prior
 * to a call to getResult, result sould not be unwrapped.
 */
Response const& ResponseHandler::getResponse() { return _response; }

/* ................................. OVERLOAD ................................*/

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
