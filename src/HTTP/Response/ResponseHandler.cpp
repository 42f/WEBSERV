#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

ResponseHandler::ResponseHandler(ReqResult requestResult, int receivedPort)
    : _port(0), _method(NULL) {
  this->init(requestResult, receivedPort);
}

/* ..............................COPY CONSTRUCTOR.............................*/

ResponseHandler::ResponseHandler(void)
    : _port(0), _method(NULL) {}

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler(void) {
  if (_method != NULL) delete _method;
}

/* ................................. METHODS .................................*/

void ResponseHandler::init(ReqResult const requestResult, int receivedPort) {
  _port = receivedPort;
  _method = NULL;
  _requestRes = requestResult;
  if (_requestRes.is_ok()) {
    _request = _requestRes.unwrap();
    switch (_request.method) {
      case methods::GET:
        _method = new (std::nothrow) GetMethod(*this);
        break;
      case methods::POST:
        _method = new (std::nothrow) PostMethod(*this);
        break;
      case methods::DELETE:
        _method = new (std::nothrow) DeleteMethod(*this);
        break;

      default:
        _method = new (std::nothrow) UnsupportedMethod(*this);
        break;
    }
    if (_method == NULL)
      GetMethod(*this).makeStandardResponse(status::InternalServerError);
  }
}

void ResponseHandler::processRequest() {
  if (_response.getState() != respState::emptyResp) {
    return;
  }
  if (_requestRes.is_err()) {
    return GetMethod(*this).makeStandardResponse(_requestRes.unwrap_err());                           // TODO check segfault ?
  }
  _serverMatch = network::ServerPool::getServerMatch(getHeader("Host"), _port);
  _loc = network::ServerPool::getLocationMatch(_serverMatch, _request.target);

  // Check if the location resolved allows the requested method
  if (_loc.get_methods().has(_request.method) == false) {
    _method->makeStandardResponse(status::MethodNotAllowed);
    std::stringstream allowed;
    allowed << _loc.get_methods();
    _response.setHeader(headerTitle::Allow, allowed.str());
    return;
  }

  // Check if the location resolved has a redirection in place
  redirect red = _loc.get_redirect();
  if (red.status != 0) {
    return _method->manageRedirect(red);
  }

  // Check if the location resolved has a redirection in place
  if (_loc.get_root().empty()) {
    return _method->makeStandardResponse(status::Unauthorized);
  }
  _method->handler();
}

// safely returns the value of a header if it exists, an empty string otherwise
std::string ResponseHandler::getHeader(const std::string& target) {
  return _request.get_header(target).unwrap_or("");
}

int ResponseHandler::doSend(int fdDest, int flags) {
#if __APPLE__
  int set = 1;
  setsockopt(fdDest, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif
  int& state = _response.getState();
  if (state == respState::emptyResp ||
      state & (respState::entirelySent | respState::ioError)) {
    return RESPONSE_SENT_ENTIRELY;
  }
  if (state & respState::cgiResp) {
    sendFromCgi(fdDest, flags);
  } else if (state & respState::fileResp)
    sendFromFile(fdDest, flags);
  else if (state & respState::buffResp)
    sendFromBuffer(fdDest, flags);
  else if (state & respState::noBodyResp)
    sendHeaders(fdDest, flags);

  if (state & (respState::entirelySent | respState::ioError))
    return RESPONSE_SENT_ENTIRELY;
  else
    return RESPONSE_AVAILABLE;
}

bool ResponseHandler::isReady() {
  int state = _response.getState();
  return state != respState::emptyResp &&
         (state & (respState::ioError | respState::entirelySent)) == false;
};

void ResponseHandler::sendHeaders(int fdDest, int flags) {
  int& state = _response.getState();
  if ((state & respState::headerSent) == false) {
    // if (_request.is_ok())
    //   std::cout << RED << "REQEST:\n"
    //             << _request.unwrap() << NC << std::endl; // TODO remove db
    // std::cout << BLUE << "RESPONSE:\n"
    //           << _response << NC << std::endl; // TODO remove db

    std::stringstream output;
    output << _response;
    if ((state & respState::cgiResp) == false) output << "\r\n";
    send(fdDest, output.str().c_str(), output.str().length(), flags);
    if (state & respState::noBodyResp)
      state |= respState::headerSent | respState::entirelySent;
    else
      state |= respState::headerSent;
  }
}

void ResponseHandler::sendFromCgi(int fdDest, int flags) {
  if ((_response.getState() & respState::headerSent) == false)
    sendHeaders(fdDest, flags);
  int cgiPipe = _response.getCgiInst().get_readable_pipe();

  if (_response.getCgiInst().status() == cgi_status::CGI_ERROR ||
      _response.getCgiInst().status() == cgi_status::SYSTEM_ERROR) {
    _response.getState() = respState::ioError;
    std::cout << "cgi error" << std::endl;
    return;
  }
  if ((_response.getState() & respState::cgiHeadersSent) == false)
    sendCgiHeaders(cgiPipe, fdDest, flags);
  doSendFromFD(cgiPipe, fdDest, flags);
}

void ResponseHandler::sendCgiHeaders(int fdSrc, int fdDest, int flags) {
  int& state = _response.getState();
  char cBuff;
  std::string output;
  int retRead = 1;
  while ((retRead = read(fdSrc, &cBuff, 1)) > 0) {
    output += cBuff;
    if (output.size() >= 3 && output[output.length() - 3] == '\n' &&
        output[output.length() - 2] == '\r' &&
        output[output.length() - 1] == '\n')
      break;
  }
  if (retRead < 0) {
    state = respState::ioError;
  } else {
    send(fdDest, output.c_str(), output.length(), flags);
    state |= respState::cgiHeadersSent;
  }
}

void ResponseHandler::sendFromFile(int fdDest, int flags) {
  if ((_response.getState() & respState::headerSent) == false)
    sendHeaders(fdDest, flags);
  if (_response.getFileInst().isGood()) {
    doSendFromFD(_response.getFileInst().getFD(), fdDest, flags);
  } else {
    _response.getState() = respState::ioError;
    return;
  }
}

void ResponseHandler::doSendFromFD(int fdSrc, int fdDest, int flags) {
  if (isReady() == false) return;
  char buff[DEFAULT_SEND_SIZE + 2];
  bzero(buff, DEFAULT_SEND_SIZE + 2);
  ssize_t retRead = 0;
  int& state = _response.getState();

  if ((retRead = read(fdSrc, buff, DEFAULT_SEND_SIZE)) < 0) {
    state = respState::ioError;
    return;
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
  } else {
    send(fdDest, buff, retRead, flags);
  }
  if (retRead == 0) {
    state |= respState::entirelySent;
  }
}

void ResponseHandler::sendFromBuffer(int fdDest, int flags) {
  std::stringstream output;

  if (_requestRes.is_ok())
  //   std::cout << RED << "REQEST:\n"
  //             << _request.unwrap() << NC << std::endl; // TODO remove db
  // std::cout << BLUE << "RESPONSE:\n"
  //           << _response << NC << std::endl; // TODO remove db

  output << _response << "\r\n" << _response.getBuffer();
  send(fdDest, output.str().c_str(), output.str().length(), flags);
  _response.getState() = respState::entirelySent;
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
