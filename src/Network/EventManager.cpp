#include "EventManager.hpp"

namespace network {
int EventManager::_kq = 0;
int EventManager::_timeout = 0;
int EventManager::_nb_Event = 0;
int EventManager::_max_ssocket = 0;
int EventManager::_total_requests = 0;
#if POLL_FN == KQUEUE
struct kevent *EventManager::_monitor;
struct kevent *EventManager::_Event;
#elif POLL_FN == SELECT
fd_set EventManager::_read_set;
fd_set EventManager::_write_set;
int EventManager::_max_fd = 0;
#endif
std::vector<Socket> EventManager::_sockets;

/*
 *	Initializes the vector of sockets and the kevent monitor list with the
 *listening sockets Also, sets the maximum listening socket number
 */

void EventManager::init(std::vector<network::ServerSocket> s) {
    int i = 0;
    for (std::vector<network::ServerSocket>::iterator it = s.begin();
         it != s.end(); it++) {
        FD_SET(it->get_id(), &EventManager::_read_set);
        EventManager::_sockets.push_back(
            Socket(it->get_id(), it->get_port(), fd_status::listener));

        if (it->get_id() > EventManager::_max_ssocket)
            EventManager::_max_ssocket = it->get_id();
        i++;
    }
    EventManager::_max_fd = EventManager::_max_ssocket;
}

EventManager::~EventManager() {}

unsigned long EventManager::get_size(void) {
    return EventManager::_sockets.size();
}

int EventManager::get_nb_events(void) { return EventManager::_nb_Event; }

int EventManager::get_total_requests(void) { return _total_requests; }

Socket &EventManager::get_socket(int index) {
    if (index < 0 || index >= static_cast<int>(_sockets.size()))
        throw(std::exception());
    return _sockets[index];
}

void EventManager::do_kevent(void) {
    struct timeval tv = {1, 0};
    FD_ZERO(&EventManager::_read_set);
    FD_ZERO(&EventManager::_write_set);

    std::vector<Socket>::iterator itr;
    for (itr = EventManager::_sockets.begin();
         itr != EventManager::_sockets.end(); ++itr) {
        if (itr->get_fd() > _max_ssocket) {
            FD_SET(itr->get_fd(), &EventManager::_read_set);
            FD_SET(itr->get_fd(), &EventManager::_write_set);
        } else
            FD_SET(itr->get_fd(), &EventManager::_read_set);
    }
    EventManager::_nb_Event =
        select(EventManager::_max_fd + 1, &EventManager::_read_set,
               &EventManager::_write_set, NULL, &tv);
    if (EventManager::_nb_Event < 0) perror("kevent");
}

void EventManager::add(int fd, int port) {
    if (fd > 0) {
        if (fd > EventManager::_max_fd) EventManager::_max_fd = fd;
        EventManager::_sockets.push_back(Socket(fd, port, fd_status::accepted));

        if (fd > _max_ssocket) {
            FD_SET(fd, &EventManager::_read_set);
            FD_SET(fd, &EventManager::_write_set);
        }
    } else {
        std::cerr << "Error: cannot add fd < 0" << std::endl;
    }
}

int EventManager::accept_request(int fd) {
    (void)fd;
    for (unsigned long i = 0; i < EventManager::_sockets.size(); i++) {
        if (EventManager::_sockets[i].get_fd() <= _max_ssocket &&
            FD_ISSET(EventManager::_sockets[i].get_fd(),
                     &EventManager::_read_set) &&
            EventManager::_sockets[i].get_status() == fd_status::listener) {
            int tmp_fd;
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            tmp_fd = accept(EventManager::_sockets[i].get_fd(),
                            (struct sockaddr *)&client_addr, &addr_len);
            if (tmp_fd < 0)
                perror("Accept");
            else {
                std::cout << EventManager::get_total_requests() << std::endl;
                add(tmp_fd, EventManager::_sockets[i].get_port());
                EventManager::_total_requests++;
            }
        }
    }
    return (0);
}

int EventManager::recv_request(int index) {
    (void)index;
    for (unsigned long i = 0; i < EventManager::_sockets.size(); i++) {
        //  std::cout << "fd to read = " << itr->second.get_fd() << " max socket
        //  = " << _max_ssocket << std::endl; std::cout << "isset = " <<
        //  FD_ISSET(itr->second.get_fd(), &_read_set) << std::endl; std::cout
        //  << "status accepted = " << std::boolalpha <<
        //  (itr->second.get_status() == fd_status::accepted)<< std::endl;
        if (EventManager::_sockets[i].get_fd() > _max_ssocket &&
            FD_ISSET(EventManager::_sockets[i].get_fd(), &_read_set) &&
            EventManager::_sockets[i].get_status() == fd_status::accepted) {
            char buffer[4096];
            int ret;

            std::cout << "L'INDEX EST " << i << std::endl;

            ret = recv(EventManager::_sockets[i].get_fd(), buffer, 4096,
                       MSG_DONTWAIT);
            std::cout << "ret read = " << ret
                      << " on fd: " << EventManager::_sockets[i].get_fd()
                      << " on port: " << EventManager::_sockets[i].get_port()
                      << std::endl;
            if (ret == 0) {
                return (-1);
            } else if (ret > 0) {
                EventManager::_sockets[i].manage_raw_request(buffer, ret);
            }
        }
    }
    return (0);
}

int EventManager::send_response(int index) {
    (void)index;
     for (unsigned long i = 0; i < EventManager::_sockets.size(); i++) {
        if (EventManager::_sockets[i].get_fd() > _max_ssocket &&
            FD_ISSET(EventManager::_sockets[i].get_fd(), &_write_set) &&
            EventManager::_sockets[i].get_status() == fd_status::read) {
            std::ostringstream buffer;

            buffer << EventManager::_sockets[i].get_response();

            unsigned long ret = send(EventManager::_sockets[i].get_fd(), buffer.str().c_str(),
                                     buffer.str().length(), 0);
            // Change condition to (if nothing else to send)
            std::cout << "ret = " << ret << " | buffer len = " << buffer.str().length() << std::endl;
            if (ret > 0) {
                std::cout << "closed " << EventManager::_sockets[i].get_fd() << std::endl;
                close(EventManager::_sockets[i].get_fd());
                EventManager::_sockets[i].set_status(fd_status::closed);
            }
        }
    }
    return (0);
}

void EventManager::resize(void) {
    for (unsigned long i = 0; i < EventManager::_sockets.size(); i++) {
        std::cout << i << ": " << EventManager::_sockets[i].get_fd() << std::endl;
        if (EventManager::_sockets[i].get_status() == fd_status::closed) {
            FD_CLR(EventManager::_sockets[i].get_fd(),
                   &EventManager::_read_set);
            FD_CLR(EventManager::_sockets[i].get_fd(),
                   &EventManager::_write_set);
            EventManager::_sockets.erase(EventManager::_sockets.begin() + i);
        }
    }
}
}  // namespace network
