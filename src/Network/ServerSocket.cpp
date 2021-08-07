#include "ServerSocket.hpp"

#include <errno.h>
#include <stdio.h>
#include <string.h>
namespace network {

ServerSocket::ServerSocket(void)
    : _type(SOCK_STREAM), _domain(AF_INET), _protocol(0), _is_good(true) {}

ServerSocket::ServerSocket(ServerSocket const &src) { *this = src; }

ServerSocket::~ServerSocket() {}

ServerSocket::ServerSocket(int const port)
    : _port(port),
      _type(SOCK_STREAM),
      _domain(AF_INET),
      _protocol(0),
      _is_good(true) {
    int enable = 1;
    if ((_id = socket(_domain, _type, _protocol)) < 0) {
        std::cerr << "Error: Cannot create ServerSocket" << std::endl;
        _is_good = false;
        return;
    }
    if (setsockopt(_id, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        std::cerr << "setsockopt(SO_REUSEADDR) failed" << std::endl;
    _address.sin_family = _domain;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_port);

    memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));
    if (_is_good == true) do_bind();
    if (_is_good == true) do_listen();
    // if (_is_good)
    //     std::cout << "ServerSocket creation and binding successfull. ServerSocket port :
    //     "
    //               << _port << " with ID : " << get_id() << std::endl;
    // else {
    //     std::cout << "ServerSocket creation and binding failed. ServerSocket port : "
    //               << _port << std::endl;
    // }
}

/***************************************************
    Overloads
***************************************************/

ServerSocket &ServerSocket::operator=(ServerSocket const &rhs) {
    if (this != &rhs) {
        _address = *(rhs.get_addr());
        _protocol = rhs.get_protocol();
        _addrlen = rhs.get_addr_len();
        _domain = rhs.get_domain();
        _type = rhs.get_type();
        _port = rhs.get_port();
        _id = rhs.get_id();
        _is_good = rhs.is_good();
    }
    return (*this);
}

/**************************************************
    Member Functions specific to ServerSockets
***************************************************/

int ServerSocket::do_bind(void) {
    _address.sin_family = _domain;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_port);
    if (bind(_id, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
        std::cerr << "Error: Cannot bind ServerSocket" << std::endl;
        // std::cerr << strerror(errno) << std::endl;
        _is_good = false;
        return (-1);
    }
    return (0);
}

int ServerSocket::do_listen(void) {
    if (listen(_id, 10) < 0) {
        std::cerr << "Error: Listen" << std::endl;
        _is_good = false;
        return (-1);
    }
    unblock();
    return (0);
}

int ServerSocket::do_accept(void) {
    socklen_t addr_len = get_addr_len();
    struct sockaddr_in client_addr;
    int new_fd;

    if ((new_fd = accept(get_id(), (struct sockaddr *)&client_addr,
                         &addr_len)) < 0) {
        _is_good = false;
        return (new_fd);
    }
    return (new_fd);
}

bool ServerSocket::is_good(void) const { return (_is_good); }

int ServerSocket::unblock(void) {
    if (_is_good == false || _id < 0) return -1;
    return (fcntl(_id, F_SETFL, O_NONBLOCK));
}

/***************************************************
    Getters
***************************************************/

int ServerSocket::get_id(void) const { return _id; }
int ServerSocket::get_protocol(void) const { return _protocol; }
int ServerSocket::get_domain(void) const { return _domain; }
int ServerSocket::get_type(void) const { return _type; }
int ServerSocket::get_port(void) const { return _port; }
int ServerSocket::get_addr_len(void) const { return (sizeof(_address)); }
struct sockaddr_in *ServerSocket::get_addr(void) const {
    return ((struct sockaddr_in *)&_address);
}

}  // namespace network

std::ostream &operator<<(std::ostream &o, network::ServerSocket const &s) {
    o << "ServerSocket " << s.get_id() << " listening on port " << s.get_port()
      << std::endl;
    return (o);
}
