#ifndef NETWORK_EVENTSMANAGER_HPP
#define NETWORK_EVENTSMANAGER_HPP

#include <sys/event.h>  // |
#include <sys/time.h>   // | kqueue(), struct kevent, kevent()
#include <sys/types.h>  // |
#include <unistd.h>     // close()

#include <map>     // stl vectors
#include <vector>  // stl vectors

#include "ServerSocket.hpp"  // network::ServerSockets
#include "Socket.hpp"        // network::Sockets, fd_status::status
#include "HTTP/RequestHandler.hpp"

namespace network {
class EventsManager {
   public:
    EventsManager(std::vector<network::ServerSocket> s);
    ~EventsManager();

    int get_kq(void) const;
    int get_size(void) const;
    int get_event_index(int fd);
    int get_event_fd(int index);
    int get_timeout(void) const;
    int get_capacity(void) const;
    int get_nb_events(void) const;
    int get_nb_ssockets(void) const;
    int get_max_ssockets(void) const;
    int get_total_requests(void) const;
    struct kevent get_event(int index) const;
    Socket  get_socket(int index) const;
    void    resize(void);

    bool is_writable(int index) const;
    bool is_readable(int index);
    bool is_acceptable(int index) const;
    bool is_request(int index) const;
    bool is_server(int index) const;

    int accept_request(int index);
    int recv_request(int index);
    int send_response(int index);

    void do_kevent(void);
    void add(int fd);

   private:
    int _kq;
    int _size;
    int _timeout;
    int _capacity;
    int _nb_events;
    int _nb_ssockets;
    int _max_ssockets;
    int _total_requests;
    struct kevent *_events;
    struct kevent *_monitor;
    std::vector<Socket> _sockets;
};
}  // namespace network

#endif