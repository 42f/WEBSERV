#ifndef NETWORK_EVENTSMANAGER_HPP
#define NETWORK_EVENTSMANAGER_HPP

#include <sys/event.h>  // |
#include <sys/time.h>   // | kqueue(), struct kevent, kevent()
#include <sys/types.h>  // |
#include <unistd.h>     // close()

#include <map>     // stl vectors
#include <vector>  // stl vectors

#include "HTTP/RequestHandler.hpp"
#include "ServerSocket.hpp"  // network::ServerSockets
#include "Socket.hpp"        // network::Sockets, fd_status::status

namespace network {
class EventsManager {
   public:
    typedef struct kevent s_kevent;
    ~EventsManager();

    static void init(std::vector<network::ServerSocket> s);

    static void resize(void);
    static void do_kevent(void);
    static int accept_request(int index);
    static int recv_request(int index);
    static int send_response(int index);

    static bool is_writable(int index);
    static bool is_readable(int index);
    static bool is_acceptable(int index);

    static int get_nb_events(void);
    static Socket &get_socket(int index);
    static int get_total_requests(void);
    static unsigned long get_size(void);
    static s_kevent get_event_struct(int id);

   private:
    static void add(int fd);
    static int _kq;
    static int _timeout;
    static int _nb_events;
    static int _max_ssocket;
    static int _total_requests;
    static s_kevent *_events;
    static s_kevent *_monitor;
    static std::map<int, Socket> _sockets;
};
}  // namespace network

#endif