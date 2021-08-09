#ifndef NETWORK_POLL_HPP
#define NETWORK_POLL_HPP

#include <errno.h>
#include <poll.h>
#include <sys/select.h>  // select()
#include <sys/socket.h>
#include <unistd.h>  // close()

#include <iostream>
#include <vector>

#include "PollFd.hpp"
#include "ServerSocket.hpp"
#include "ThreadPool.hpp"

namespace network {
class Poll {
   public:
    Poll(std::vector<network::ServerSocket> s, int _size_tpool);
    ~Poll();

    void run_servers(std::vector<network::ServerSocket> s);
    void do_poll(void);
    void check_sockets(std::vector<network::ServerSocket> s);
    void check_requests(void);
    void send_response(void);

   private:
    Poll(void);
    PollFd _fds;
    ThreadPool _tpool;
};
}  // namespace network

std::ostream &operator<<(std::ostream &o, network::Poll const &p);
void *ok(void *args);

#endif