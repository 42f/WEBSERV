#include "Socket.hpp"

namespace network {

/***************************************************
    Constructors & destructor
***************************************************/

Socket::Socket(int fd, int port, fd_status::status status)
    : _fd(fd),
      _port(port),
      _has_events(false),
      _is_processed(false),
      _status(status),
      _res(result_type::err(status::None)) {
    if (fd < 0) {
        _status = fd_status::error;
    }
}

Socket::Socket(void)
    : _has_events(false), _res(result_type::err(status::None)) {}

Socket::Socket(Socket const &src) { *this = src; }

Socket::~Socket(void) {}

/***************************************************
    Operator Overload
***************************************************/

Socket &Socket::operator=(Socket const &rhs) {
    if (this != &rhs) {
        _fd = rhs._fd;
        _port = rhs._port;
        _flags = rhs._flags;
        _status = rhs._status;
        _has_events = rhs._has_events;
        _is_processed = false;
    }
    return *this;
}

/***************************************************
    Setters
***************************************************/

void Socket::set_flags(int flags) { _flags = flags; }
void Socket::set_has_events(bool value) { _has_events = value; }
void Socket::set_status(fd_status::status status) { _status = status; }

/***************************************************
    Getters
***************************************************/

int Socket::get_fd() const { return _fd; }
int Socket::get_port(void) const { return _port; }
int Socket::get_flags(void) const { return _flags; }
bool Socket::has_events(void) const { return _has_events; }
fd_status::status Socket::get_status(void) const { return _status; }
Response const &Socket::get_response(void) const { return _response; }

/***************************************************
    Member functions
***************************************************/

void Socket::manage_raw_request(char *buffer, int size) {
    _res = _request_handler.update(buffer, size);
    if (_res.is_ok()) {
        set_status(fd_status::read);
        _response_handler.init(_res, _port);
    }
}

int Socket::manage_response() {
    if (_is_processed == false) {
        _response_handler.processRequest();
        _is_processed = true;
    }
    return (_response_handler.doSend(_fd));
}

}  // namespace network