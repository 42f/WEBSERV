#include "Socket.hpp"

namespace network {

/***************************************************
    Constructors & destructor
***************************************************/

Socket::Socket(int fd, int port, char *client_ip, int status)
    : _fd(fd),
      _port(port),
      _status(status),
      _res(result_type::err(status::None)),
      _response_handler(_request_handler, 0) {
  if (fd < 0) {
    _status = fd_status::error;
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

    _is_processed = false;
    _res = result_type::err(status::None);
  }
  return *this;
}

/***************************************************
    Setters
***************************************************/

void Socket::set_status(int status) { _status |= status; }
void Socket::unset_status(int status) { _status &= ~status; }

/***************************************************
    Getters
***************************************************/

int Socket::get_skt_fd() const { return _fd; }
// int Socket::get_o_fd() const { return _ofd; }
int Socket::get_o_fd() const { return _response_handler.getResponse().getOutputFd(); }
int Socket::get_u_fd() const { return _response_handler.getResponse().getUploadFd(); }
int Socket::get_port(void) const { return _port; }
int Socket::get_status(void) const { return _status; }
std::string Socket::get_client_ip(void) const { return _client_ip; }

/***************************************************
    Member functions
***************************************************/

void Socket::manage_raw_request(char *buffer, int size) {
  _res = _request_handler.update(buffer, size, _port);
  if (_res.is_ok() || _res.unwrap_err() != status::Incomplete) {
    unset_status(fd_status::skt_readable);
    set_status(fd_status::skt_writable);
    if (_res.is_ok()) _res.unwrap().set_client_ip(_client_ip);
    _response_handler.init(_request_handler, _port);
  }
}

void Socket::process_request(fd_set const & writeSet) {
  Response const & resp = _response_handler.getResponse();

  if (_is_processed == false) {
    _ofd = _response_handler.processRequest();
    setOfdStatus();
    _is_processed = true;
  } else if (FD_ISSET(resp.getUploadFd(), &writeSet)) {
    _response_handler.doWriteBody();
  }
}

void Socket::setOfdStatus() {
  if (_ofd == RESPONSE_NO_FD)
    _status |= fd_status::ofd_no_need;
  else
    _status |= fd_status::ofd_usable;
}

int Socket::do_send() { return _response_handler.doSend(_fd); }

}  // namespace network
