#include "Socket.hpp"

namespace network {

/***************************************************
    Constructors & destructor
***************************************************/

Socket::Socket(int fd, fd_status::status status) : _fd(fd), _status(status) {
    if (fd < 0) {
        _status = fd_status::error;
    }
}

Socket::~Socket(void) {}

/***************************************************
    Setters & Getters
***************************************************/

int Socket::get_fd() const { return _fd; }
fd_status::status Socket::get_status() const { return _status; }
int Socket::get_flags(void) const { return _flags; }
void Socket::set_flags(int flags) { _flags = flags; }
bool Socket::has_events(void) const { return _has_events; }
void Socket::set_has_events(bool value) { _has_events = value; }

void Socket::set_status(fd_status::status status) { _status = status; }

void Socket::manage_raw_request(char *buffer, int size) {
    std::cout << size << std::endl;
    Result<Request, status::StatusCode> _res = _handler.update(buffer, size);
    if (_res.is_ok()) {
        Request req;
        req = _res.unwrap();
    }
}

}  // namespace network