#include "ServerSocket.hpp"

#include <errno.h>
#include <stdio.h>
#include <string.h>
namespace network {

/*
 * ServerSocket uses TCP for socket connections (IPv4 + SOCK_STREAM)
 *  The address format used it the Internet Protocol Adress AF_INET (= IPv4)
 *  The protocol for packet transport is SOCK_STREAM (used by TCP)
 *  The protocol family is PF_INET (used for IP, TCP/IP, UDP/IP)
 *
 * ServerSocket uses non-blocking & reusable listening sockets
 *  All reading/writing sockets inherit their non-blocking parameter from
 *  listening sockets
 *
 * ServerSocket are used as listening socket. Upon instanciation they are
 * create, binded and are listening on any ip address on the specified port
 *
 * If ServerSocket cannot be created/binded/listening webserv will exit with
 * status 1
 */

ServerSocket::ServerSocket(int const port)
    : _port(port), _type(SOCK_STREAM), _domain(AF_INET), _protocol(0) {
    if ((_id = socket(_domain, _type, _protocol)) < 0) {
        perror("socket()");
        exit(1);
    }

    int enable = 1;
    if (setsockopt(_id, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("socket()");
        exit(1);
    }

    _address.sin_family = _domain;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_port);

    memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));

    do_bind();
    do_listen();
}

ServerSocket::ServerSocket(ServerSocket const &src) { *this = src; }

ServerSocket::~ServerSocket() {}

/***************************************************
    Operator Overloads
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
    }
    return (*this);
}

/**************************************************
    Member Functions specific to ServerSockets
***************************************************/

void ServerSocket::do_bind(void) {
    if (bind(_id, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
        perror("Cannot bind ServerSocket");
        exit(1);
    }
}

void ServerSocket::do_listen(void) {
    if (listen(_id, 1024) < 0) {
        perror("Cannot bind ServerSocket");
        exit(1);
    }
    if (unblock() < 0) {
        perror("Unblocking socket");
        exit(1);
    }
}

int ServerSocket::unblock(void) {
    if (_id < 0) return -1;
    return (fcntl(_id, F_SETFL, O_NONBLOCK));
}

/***************************************************
    Getters
***************************************************/

int ServerSocket::get_id(void) const { return _id; }
int ServerSocket::get_type(void) const { return _type; }
int ServerSocket::get_port(void) const { return _port; }
int ServerSocket::get_domain(void) const { return _domain; }
int ServerSocket::get_protocol(void) const { return _protocol; }
int ServerSocket::get_addr_len(void) const { return (sizeof(_address)); }
struct sockaddr_in *ServerSocket::get_addr(void) const {
    return ((struct sockaddr_in *)&_address);
}

/***************************************************
    Setters
***************************************************/

void ServerSocket::set_port(int port) { _port = port; }

}  // namespace network

std::ostream &operator<<(std::ostream &o, network::ServerSocket const &s) {
    o << "ServerSocket " << s.get_id() << " listening on port " << s.get_port()
      << std::endl;
    return (o);
}
