#include "ResponseHandler.hpp"

/* ............................... CONSTRUCTOR ...............................*/

ResponseHandler::ResponseHandler(RequestHandler & reqHandler, int receivedPort)
    : _requestHandler(reqHandler), _port(0), _method(NULL) {
  this->init(reqHandler, receivedPort);
}

/* ..............................COPY CONSTRUCTOR.............................*/

// ResponseHandler::ResponseHandler(void) : _reqHandler(RequestHandler()), _port(0), _method(NULL) {}

/* ................................ DESTRUCTOR ...............................*/

ResponseHandler::~ResponseHandler(void) {
  if (_method != NULL) delete _method;
}

/* ................................. METHODS .................................*/

void ResponseHandler::init(RequestHandler & reqHandler, int receivedPort) {
  if (_method != NULL)
    delete _method;
  _method = NULL;

  _port = receivedPort;
  _requestHandler = reqHandler;
  if (_requestHandler._req.is_ok()) {
    _req = _requestHandler._req.unwrap();
    switch (_req.method) {
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
        break;
    }
    if (_method == NULL)
      GetMethod(*this).makeStandardResponse(status::InternalServerError);
  }
}

void ResponseHandler::processRequest() {
  if (_resp.getState() != respState::emptyResp) {
    return;
  }
  if (_requestHandler._req.is_err()) {
    return GetMethod(*this).makeStandardResponse(_requestHandler._req.unwrap_err());
  }
  std::string host = getReqHeader("Host");
  if (host.empty())
    return GetMethod(*this).makeStandardResponse(status::BadRequest);
  _serv = network::ServerPool::getServerMatch(host, _port);
  _loc = network::ServerPool::getLocationMatch(_serv, _req.target);

  // Check if the location resolved allows the requested method
  if (_loc.get_methods().has(_req.method) == false) {
    _method->makeStandardResponse(status::MethodNotAllowed);
    std::stringstream allowed;
    allowed << _loc.get_methods();
    _resp.setHeader(headerTitle::Allow, allowed.str());
    return;
  }

  // If any payload, check if acceptable size
  if (_loc.get_body_size() < _req.get_body().size()) {
    return _method->makeStandardResponse(status::PayloadTooLarge);
  }

  // Check if the location resolved has a redirection in place
  redirect red = _loc.get_redirect();
  if (red.status != 0) {
    return _method->manageRedirect(red);
  }

  if (_loc.get_root().empty()) {
    return _method->makeStandardResponse(status::Forbidden);
  }
  _method->handler();
}

// safely returns the value of a header if it exists, an empty string otherwise
std::string ResponseHandler::getReqHeader(const std::string& target) {
  return _req.get_header(target).unwrap_or("");
}

int ResponseHandler::doSend(int fdDest, int flags) {
#if __APPLE__
  int set = 1;
  setsockopt(fdDest, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif
  int& state = _resp.getState();
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
  int state = _resp.getState();
  return state != respState::emptyResp &&
         (state & (respState::ioError | respState::entirelySent)) == false;
};

void ResponseHandler::sendHeaders(int fdDest, int flags) {
  int& state = _resp.getState();
  if ((state & respState::headerSent) == false) {
    // if (_requestHandler._req.is_ok())
    //   std::cout << RED << "REQUEST:\n"
    //             << _requestHandler._req.unwrap() << NC << std::endl; // TODO remove db
    // std::cout << BLUE << "RESPONSE:\n"
    //           << _resp << NC << std::endl; // TODO remove db

    std::stringstream output;
    output << _resp;
    if ((state & respState::cgiResp) == false) output << "\r\n";
    send(fdDest, output.str().c_str(), output.str().length(), flags);
    if (state & respState::noBodyResp)
      state |= respState::headerSent | respState::entirelySent;
    else
      state |= respState::headerSent;
  }
}

void ResponseHandler::sendFromCgi(int fdDest, int flags) {
  if ((_resp.getState() & respState::headerSent) == false)
    sendHeaders(fdDest, flags);
  int cgiPipe = _resp.getCgiInst().get_readable_pipe();

  if (_resp.getCgiInst().status() == cgi_status::CGI_ERROR ||
      _resp.getCgiInst().status() == cgi_status::SYSTEM_ERROR) {
    _resp.getState() = respState::ioError;
    std::cout << "cgi error" << std::endl;
    return;
  }
  // TODO select
  // TODO select
  if ((_resp.getState() & respState::cgiHeadersSent) == false)
    sendCgiHeaders(cgiPipe, fdDest, flags);
  if (doSendFromFD(cgiPipe, fdDest, flags) < 1)
    close(cgiPipe);
}

void ResponseHandler::sendCgiHeaders(int fdSrc, int fdDest, int flags) {
  int& state = _resp.getState();
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
  if ((_resp.getState() & respState::headerSent) == false)
    sendHeaders(fdDest, flags);
  if (_resp.getFileInst().isGood()) {
    doSendFromFD(_resp.getFileInst().getFD(), fdDest, flags);
  } else {
    _resp.getState() = respState::ioError;
    return;
  }
}

int ResponseHandler::doSendFromFD(int fdSrc, int fdDest, int flags) {
  if (isReady() == false) return 1 ; // todo remove
  char buff[DEFAULT_SEND_SIZE + 2];
  bzero(buff, DEFAULT_SEND_SIZE + 2);
  ssize_t ret = 0;
  int& state = _resp.getState();

  if ((ret = read(fdSrc, buff, DEFAULT_SEND_SIZE)) < 0) {
    state = respState::ioError;
    return -1;
  }
  if (state & respState::chunkedResp) {
    std::stringstream chunkSize;
    chunkSize << std::hex << ret << "\r\n";
    std::string chunkData(chunkSize.str());
    chunkData.reserve(chunkData.length() + DEFAULT_SEND_SIZE + 2);
    buff[ret + 0] = '\r';
    buff[ret + 1] = '\n';
    chunkData.insert(chunkData.end(), buff, buff + ret + 2);
    ret = send(fdDest, chunkData.data(), chunkData.length(), flags);
  } else {
    ret = send(fdDest, buff, ret, flags);
  }
  if (ret == 0) {
    state |= respState::entirelySent;
  }
  return ret;
}

void ResponseHandler::sendFromBuffer(int fdDest, int flags) {
  std::stringstream output;

  // if (_requestHandler._req.is_ok())
  //     std::cout << RED << "REQUEST:\n"
  //               << _requestHandler._req.unwrap() << NC << std::endl; // TODO remove db
  //   std::cout << BLUE << "RESPONSE:\n"
  //             << _resp << NC << std::endl; // TODO remove db

    output << _resp << "\r\n" << _resp.getBuffer();
  send(fdDest, output.str().c_str(), output.str().length(), flags);
  _resp.getState() = respState::entirelySent;
}

/* ................................. ACCESSOR ................................*/

/*
 * Returns the result processed. If no call to processRequest was made prior
 * to a call to getResult, result sould not be unwrapped.
 */
Response const& ResponseHandler::getResponse()const  { return _resp; }
Request const& ResponseHandler::getRequest() const { return _req; }

/* ................................. OVERLOAD ................................*/

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
