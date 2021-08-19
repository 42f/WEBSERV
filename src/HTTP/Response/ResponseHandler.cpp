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
      A_Method::makeErrorResponse(_response, status::Unauthorized, serverMatch);
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
  if (state & respState::cgiResp) {
    return sendFromCgi(fdDest, flags);
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
    if ((_response.getState() & respState::cgiResp) == false) output << "\r\n";
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
         (respState::fileResp | respState::cgiResp | respState::buffResp);
};

int ResponseHandler::sendFromCgi(int fdDest, int flags) {
  // TODO implement
  sendHeaders(fdDest, flags);
  int retSend = 0;
  cgi_status::status status = _response.getCgiInst().status();

  if (status == cgi_status::ERROR) {
    _response.getState() = respState::readError;
    return RESPONSE_READ_ERROR;
  } else {
    retSend =
        doSendFromFD(_response.getCgiInst().get_readable_pipe(), fdDest, flags);
  }

  // NOT GOOD : done only means execve has finished executing the command,
  // but there might be something to read on the pipe still
  if (retSend == 0) {
    _response.getState() = respState::entirelySent;
    return RESPONSE_SENT_ENTIRELY;
  }
  return retSend;
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
  int& state = _response.getState();

  if (state & respState::cgiResp &&
      (state & respState::cgiHeadersSent) == false) {
    return sendCgiHeaders(fdSrc, fdDest, flags);
  }

  if ((retRead = read(fdSrc, buff, DEFAULT_SEND_SIZE)) < 0) {
    state = respState::readError;
    return (RESPONSE_READ_ERROR);
  }

  if (state & respState::chunkedResp) {
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

int ResponseHandler::sendCgiHeaders(int fdSrc, int fdDest, int flags) {
  char cBuff;
  std::string output;
  int retRead = 1;
  while ((retRead = read(fdSrc, &cBuff, 1)) > 0) {
    output += cBuff;
    if (output.size() >= 3 && output[output.length() - 3] == '\n' &&
        output[output.length() - 2] == '\r' && output[output.length() - 1] == '\n')
      break;
  }
  if (retRead < 0) return RESPONSE_READ_ERROR;
  send(fdDest, output.c_str(), output.length(), flags);
  _response.getState() |= respState::cgiHeadersSent;
  return output.length();
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
