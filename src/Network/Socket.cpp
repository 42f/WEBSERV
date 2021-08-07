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

void Socket::set_status(fd_status::status status) { _status = status; }

}  // namespace network