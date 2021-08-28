#include "Socket.hpp"

namespace network {

/***************************************************
    Constructors & destructor
***************************************************/

Socket::Socket(int fd, int port, char *client_ip, fd_status::status status)
    : _fd(fd),
      _port(port),
      _status(status),
      _res(result_type::err(status::None)),
      _response_handler(_request_handler, 0) {
  if (fd < 0) {
    _status = fd_status::closed;
  }
  if (client_ip != NULL) {
    _client_ip = std::string(client_ip);
  } else {
    _client_ip = std::string("");
  }
}

Socket::Socket(Socket const &src) : _response_handler(_request_handler, 0) {
  *this = src;
}

Socket::~Socket(void) {}

/***************************************************
    Operator Overload
***************************************************/

Socket &Socket::operator=(Socket const &rhs) {
  if (this != &rhs) {
    _fd = rhs._fd;
    _port = rhs._port;
    _status = rhs._status;
    _client_ip = rhs._client_ip;

    _has_events = false;
    _is_processed = false;
    _res = result_type::err(status::None);
  }
  return *this;
}

/***************************************************
    Setters
***************************************************/

void Socket::set_has_events(bool value) { _has_events = value; }
void Socket::set_status(fd_status::status status) { _status = status; }

/***************************************************
    Getters
***************************************************/

int Socket::get_fd() const { return _fd; }
int Socket::get_port(void) const { return _port; }
bool Socket::has_events(void) const { return _has_events; }
fd_status::status Socket::get_status(void) const { return _status; }
Response const &Socket::get_response(void) const { return _response; }
std::string Socket::get_client_ip(void) const { return _client_ip; }

/***************************************************
    Member functions
***************************************************/

void Socket::manage_raw_request(char *buffer, int size) {
  _res = _request_handler.update(buffer, size);
  if (_res.is_ok()) {
    set_status(fd_status::read);
    _res.unwrap().set_client_ip(_client_ip);
    _response_handler.init(_request_handler, _port);
  }
}

int Socket::manage_response() {
  if (_is_processed == false) {
    _response_handler.processRequest();
    _is_processed = true;
  }
  return _response_handler.doSend(_fd);
}

}  // namespace network
