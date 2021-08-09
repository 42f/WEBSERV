#ifndef NETWORK_POLLFD_HPP
#define NETWORK_POLLFD_HPP

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include <vector>

#include "ServerSocket.hpp"
#include "Socket.hpp"

namespace network {
class PollFd {
   public:
    PollFd(std::vector<network::ServerSocket> s);
    PollFd(void);
    ~PollFd();
    void add(network::Socket socket);
    void resize(void);
    void do_poll(void);
    bool is_acceptable(int index);
    bool is_readable(int index);
    bool is_writable(int index);
    void close(int fd);

    int get_nb_ssocket(void) const;
    int get_max_ssocket(void) const;
    int get_size(void) const;
    int get_timeout(void) const;
    int get_capacity(void) const;
    int get_nb_ready(void) const;
    int get_request_nb(void) const;
    int get_fd(int index) const;
    struct kevent get_events(int index) const;
    // int get_revents(int index) const;

    void set_nb_ssocket(int value);
    void set_size(int value);
    void set_timeout(int value);
    void set_capacity(int value);
    void set_nb_ready(int value);
    void set_request_nb(int value);
    void set_status(int index, fd_status::status status);

   private:
    int _timeout;
    int _capacity;
    int _size;
    int _nb_socket;
    int _nb_ready;
    int _request_nb;
    int _kq;
    int _max_socket;
    struct kevent *_monitor;
    struct kevent *_events;
    std::vector<network::Socket> _sockets;
};
// int _timeout; 			timeout value for poll()
// int _capacity;			current capacity of the instance (works
// like a vector) int _size;				number of active sockets
// int _nb_socket;			number of active listening sockets
// (server sockets)
// int _nb_ready;			number of socket with returned event,
// after poll() int _request_nb;			total of requests since
// server is
// running struct pollfd *_fds;		array of pollfds (fd, events, revents)
}  // namespace network
#endif