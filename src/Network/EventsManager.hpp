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
    // EventsManager(std::vector<network::ServerSocket> s);
    ~EventsManager();

    static int get_kq(void);
    static int get_size(void);
    static int get_timeout(void);
    static int get_capacity(void);
    static int get_nb_events(void);
    static int get_nb_ssockets(void);
    static int get_max_ssockets(void);
    static int get_event_index(int fd);
    static int get_event_fd(int index);
    static int get_total_requests(void);

    static void add(int fd);
    static void resize(void);
    static void do_kevent(void);
    static bool is_server(int index);
    static bool is_request(int index);
    static bool is_writable(int index);
    static int recv_request(int index);
    static bool is_readable(int index);
    static Socket get_socket(int index);
    static int send_response(int index);
    static s_kevent get_event(int index);
    static bool is_acceptable(int index);
    static int accept_request(int index);
    static void init(std::vector<network::ServerSocket> s);

   private:
    static int _kq;
    static int _size;
    static int _timeout;
    static int _capacity;
    static int _nb_events;
    static int _nb_ssockets;
    static int _max_ssockets;
    static int _total_requests;
    static s_kevent *_events;
    static s_kevent *_monitor;
    static std::vector<Socket> _sockets;
};
}  // namespace network

#endif