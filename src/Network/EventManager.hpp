#ifndef NETWORK_EVENTMANAGER_HPP
#define NETWORK_EVENTMANAGER_HPP

#define KQUEUE 0
#define SELECT 1
#define POLL_FN SELECT

#define ACCEPTFL 1
#define READFL 2
#define WRITEFL 4
#define DEFFL 0

#include <stdio.h>
#include <stdlib.h>
#if POLL_FN == KQUEUE
#include <sys/event.h>  // |
#endif
#include <sys/time.h>   // | kqueue(), struct kevent, kevent()
#include <sys/types.h>  // |
#include <unistd.h>     // close()

#include <map>     // stl vectors
#include <vector>  // stl vectors

#include "HTTP/RequestHandler.hpp"
#include "ServerSocket.hpp"  // network::ServerSockets
#include "Socket.hpp"        // network::Sockets, fd_status::status

namespace network {
class EventManager {
   public:
    ~EventManager();

    static void init(std::vector<network::ServerSocket> s);

    static void resize(void);
    static void do_kevent(void);
    static int accept_request(int index);
    static int recv_request(int index);
    static int send_response(int index);

    static int get_nb_events(void);
    static Socket &get_socket(int index);
    static int get_total_requests(void);
    static unsigned long get_size(void);

   private:
    static void add(int fd, int port);
    static int _kq;
    static int _max_fd;
    static int _timeout;
    static int _nb_Event;
    static int _max_ssocket;
    static int _total_requests;
    static fd_set _read_set;
    static fd_set _write_set;
    static std::vector<Socket> _sockets;
};
}  // namespace network

#endif