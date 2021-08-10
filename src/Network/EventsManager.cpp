#include "EventsManager.hpp"

namespace network {
int EventsManager::_kq = 0;
int EventsManager::_timeout = 0;
int EventsManager::_nb_events = 0;
int EventsManager::_max_ssocket = 0;
int EventsManager::_total_requests = 0;
#if POLL_FN == KQUEUE
struct kevent *EventsManager::_monitor;
struct kevent *EventsManager::_events;
#elif POLL_FN == SELECT
fd_set EventsManager::_read_set;
fd_set EventsManager::_write_set;
int EventsManager::_max_fd = 0;
#endif
std::map<int, Socket> EventsManager::_sockets;

/*
 *	Initializes the vector of sockets and the kevent monitor list with the
 *listening sockets Also, sets the maximum listening socket number
 */

#if POLL_FN == KQUEUE
void EventsManager::init(std::vector<network::ServerSocket> s) {
    EventsManager::_monitor = new struct kevent[s.size()];
    EventsManager::_events = new struct kevent[s.size()];

    if ((EventsManager::_kq = kqueue()) < 0) perror("Kqueue");

    int i = 0;
    for (std::vector<network::ServerSocket>::iterator it = s.begin();
         it != s.end(); it++) {
        EV_SET(&EventsManager::_monitor[i], it->get_id(), EVFILT_READ,
               EV_ADD | EV_ENABLE, 0, 0, 0);

        EventsManager::_sockets.insert(std::pair<int, Socket>(
            it->get_id(), Socket(it->get_id(), fd_status::listener)));

        if (it->get_id() > EventsManager::_max_ssocket)
            EventsManager::_max_ssocket = it->get_id();
        i++;
    }
}

EventsManager::~EventsManager() {
    delete[] EventsManager::_monitor;
    delete[] EventsManager::_events;
}

unsigned long EventsManager::get_size(void) {
    return EventsManager::_sockets.size();
}

int EventsManager::get_nb_events(void) { return EventsManager::_nb_events; }

int EventsManager::get_total_requests(void) { return _total_requests; }

Socket &EventsManager::get_socket(int index) {
    if (index < 0 || index >= static_cast<int>(_sockets.size()))
        throw(std::exception());
    return _sockets[index];
}

EventsManager::s_kevent EventsManager::get_event_struct(int index) {
    return (_events[index]);
}

bool EventsManager::is_readable(int index) {
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_READ &&
        EventsManager::_sockets[index].get_status() == fd_status::accepted) {
        return true;
    }
    return (false);
}

bool EventsManager::is_writable(int index) {
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_WRITE &&
        EventsManager::_sockets[index].get_status() == fd_status::read) {
        return true;
    }
    return (false);
}

bool EventsManager::is_acceptable(int index) {
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_READ &&
        EventsManager::_sockets[index].get_status() == fd_status::listener) {
        return true;
    }
    return false;
}

void EventsManager::do_kevent(void) {
    delete[] EventsManager::_events;
    EventsManager::_events = new struct kevent[_sockets.size()];

    // std::map<int, Socket>::iterator itr;
    // std::cout << "\nThe map _sockets is : \n";
    // std::cout << "\tKEY\tELEMENT\n";
    // for (itr = EventsManager::_sockets.begin();
    //      itr != EventsManager::_sockets.end(); ++itr) {
    //     std::cout << '\t' << itr->first << '\t' << itr->second.get_fd() <<
    //     '\n';
    // }
    // std::cout << std::endl;

    struct timespec timeout = {0, 10000000};
    EventsManager::_nb_events =
        kevent(EventsManager::_kq, EventsManager::_monitor, _sockets.size(),
               EventsManager::_events, _sockets.size(), &timeout);
    if (EventsManager::_nb_events < 0) perror("kevent");

    for (int i = 0; i < EventsManager::_nb_events; i++) {
        int index = EventsManager::_events[i].ident;
        EventsManager::_sockets[index].set_has_events(true);
        EventsManager::_sockets[index].set_flags(
            EventsManager::_events[i].flags);
    }
}

void EventsManager::add(int fd) {
    if (fd > 0) {
        EventsManager::_sockets.insert(
            std::pair<int, Socket>(fd, Socket(fd, fd_status::accepted)));
        delete[] _monitor;
        _monitor = new struct kevent[_sockets.size()];

        std::map<int, Socket>::iterator itr;
        int i = 0;
        for (itr = EventsManager::_sockets.begin();
             itr != EventsManager::_sockets.end(); ++itr) {
            if (itr->second.get_fd() > _max_ssocket)
                EV_SET(&EventsManager::_monitor[i], itr->second.get_fd(),
                       EVFILT_READ | EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
            else
                EV_SET(&EventsManager::_monitor[i], itr->second.get_fd(),
                       EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
            i++;
        }
    } else {
        std::cerr << "Error: cannot add fd < 0" << std::endl;
    }
}

int EventsManager::accept_request(int fd) {
    int tmp_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    tmp_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
    if (tmp_fd < 0)
        perror("Accept");
    else {
        std::cout << EventsManager::get_total_requests() << std::endl;
        add(tmp_fd);
        EventsManager::_total_requests++;
    }
    return (tmp_fd);
}

int EventsManager::recv_request(int index) {
    char buffer[4096];
    int ret;

    ret = recv(EventsManager::_sockets[index].get_fd(), buffer, 4096, 0);
    if (ret == 0) {
        return (-1);
    } else {
        EventsManager::_sockets[index].manage_raw_request(buffer, ret);
        EventsManager::_sockets[index].set_status(fd_status::read);
    }
    return (0);
}

int EventsManager::send_response(int index) {
    std::ostringstream buffer;

    buffer << EventsManager::_sockets[index].get_response();

    unsigned long ret = send(EventsManager::_sockets[index].get_fd(),
                             buffer.str().c_str(), buffer.str().length(), 0);
    while (ret > 0 && ret < buffer.str().length()) {
        ret += send(EventsManager::_sockets[index].get_fd(),
                    buffer.str().c_str(), buffer.str().length(), 0);
    }

    if (ret > 0) {
        close(EventsManager::_sockets[index].get_fd());
        EventsManager::_sockets[index].set_status(fd_status::closed);
    }

    return (0);
}

void EventsManager::resize(void) {
    std::map<int, Socket>::iterator itr;
    std::map<int, Socket>::iterator next;
    for (itr = EventsManager::_sockets.begin(), next = itr;
         itr != EventsManager::_sockets.end(); itr = next) {
        ++next;
        if (itr->second.get_status() == fd_status::closed) {
            EventsManager::_sockets.erase(itr);
        }
    }

    delete[] _monitor;
    _monitor = new struct kevent[_sockets.size()];
    int i = 0;
    for (itr = EventsManager::_sockets.begin();
         itr != EventsManager::_sockets.end(); ++itr) {
        EV_SET(&EventsManager::_monitor[i], itr->second.get_fd(), EVFILT_READ,
               EV_ADD | EV_ENABLE, 0, 0, 0);
        i++;
    }
}
#elif POLL_FN == SELECT

void EventsManager::init(std::vector<network::ServerSocket> s) {
    int i = 0;
    for (std::vector<network::ServerSocket>::iterator it = s.begin();
         it != s.end(); it++) {
        FD_SET(it->get_id(), &EventsManager::_read_set);
        EventsManager::_sockets.insert(std::pair<int, Socket>(
            it->get_id(), Socket(it->get_id(), fd_status::listener)));

        if (it->get_id() > EventsManager::_max_ssocket)
            EventsManager::_max_ssocket = it->get_id();
        i++;
    }
    EventsManager::_max_fd = EventsManager::_max_ssocket;
}

EventsManager::~EventsManager() {}

unsigned long EventsManager::get_size(void) {
    return EventsManager::_sockets.size();
}

int EventsManager::get_nb_events(void) { return EventsManager::_nb_events; }

int EventsManager::get_total_requests(void) { return _total_requests; }

Socket &EventsManager::get_socket(int index) {
    if (index < 0 || index >= static_cast<int>(_sockets.size()))
        throw(std::exception());
    return _sockets[index];
}

void EventsManager::do_kevent(void) {

    struct timeval tv = {1, 0};
    FD_ZERO(&EventsManager::_read_set);
    FD_ZERO(&EventsManager::_write_set);

    std::map<int, Socket>::iterator itr;
    for (itr = EventsManager::_sockets.begin();
         itr != EventsManager::_sockets.end(); ++itr) {
        if (itr->second.get_fd() > _max_ssocket) {
            FD_SET(itr->second.get_fd(), &EventsManager::_read_set);
            FD_SET(itr->second.get_fd(), &EventsManager::_write_set);
        } else
            FD_SET(itr->second.get_fd(), &EventsManager::_read_set);
    }
    EventsManager::_nb_events =
        select(EventsManager::_max_fd + 1, &EventsManager::_read_set,
               &EventsManager::_write_set, NULL, &tv);
    if (EventsManager::_nb_events < 0) perror("kevent");

    for (itr = EventsManager::_sockets.begin();
         itr != EventsManager::_sockets.end(); ++itr) {
        if (FD_ISSET(itr->first, &EventsManager::_read_set)) {
            itr->second.set_has_events(true);
            itr->second.set_flags(READFL);
        }
    }
}

void EventsManager::add(int fd) {
    if (fd > 0) {
        if (fd > EventsManager::_max_fd) EventsManager::_max_fd = fd;
        EventsManager::_sockets.insert(
            std::pair<int, Socket>(fd, Socket(fd, fd_status::accepted)));

        if (fd > _max_ssocket) {
            FD_SET(fd, &EventsManager::_read_set);
            FD_SET(fd, &EventsManager::_write_set);
        } else
            FD_SET(fd, &EventsManager::_read_set);

    } else {
        std::cerr << "Error: cannot add fd < 0" << std::endl;
    }
}

int EventsManager::accept_request(int fd) {
    (void)fd;
    std::map<int, Socket>::iterator itr;
    int i = 0;
    for (itr = EventsManager::_sockets.begin();
         itr != EventsManager::_sockets.end(); ++itr) {
        if (itr->second.get_fd() <= _max_ssocket &&
            FD_ISSET(itr->second.get_fd(), &EventsManager::_read_set) &&
            itr->second.get_status() == fd_status::listener) {
            int tmp_fd;
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            tmp_fd = accept(itr->second.get_fd(),
                            (struct sockaddr *)&client_addr, &addr_len);
            if (tmp_fd < 0)
                perror("Accept");
            else {
                std::cout << EventsManager::get_total_requests() << std::endl;
                add(tmp_fd);
                EventsManager::_total_requests++;
            }
        } else
            FD_SET(itr->second.get_fd(), &EventsManager::_read_set);
        i++;
    }
    return (0);
}

int EventsManager::recv_request(int index) {
    (void)index;
    std::map<int, Socket>::iterator itr;
    for (itr = EventsManager::_sockets.begin();
         itr != EventsManager::_sockets.end(); ++itr) {
        if (itr->second.get_fd() > _max_ssocket &&
            FD_ISSET(itr->second.get_fd(), &_read_set) &&
            itr->second.get_status() == fd_status::accepted) {
            char buffer[4096];
            int ret;

            ret = recv(itr->second.get_fd(), buffer, 4096, 0);
            if (ret == 0) {
                return (-1);
            } else if (ret > 0) {
                // std::cout << "RET = " << ret << std::endl;
                itr->second.manage_raw_request(buffer, ret);
                itr->second.set_status(fd_status::read);
            }
        }
    }
    return (0);
}

int EventsManager::send_response(int index) {
    (void)index;
    std::map<int, Socket>::iterator itr;
    for (itr = EventsManager::_sockets.begin();
         itr != EventsManager::_sockets.end(); ++itr) {
        if (itr->second.get_fd() > _max_ssocket &&
            FD_ISSET(itr->second.get_fd(), &_write_set) &&
            itr->second.get_status() == fd_status::read) {
            std::ostringstream buffer;

            buffer << itr->second.get_response();

            unsigned long ret = send(itr->second.get_fd(), buffer.str().c_str(),
                                     buffer.str().length(), 0);
            while (ret > 0 && ret < buffer.str().length()) {
                ret += send(itr->second.get_fd(), buffer.str().c_str(),
                            buffer.str().length(), 0);
            }

            if (ret > 0) {
                close(itr->second.get_fd());
                itr->second.set_status(fd_status::closed);
            }
        }
    }
    return (0);
}

void EventsManager::resize(void) {
    std::map<int, Socket>::iterator itr;
    std::map<int, Socket>::iterator next;
    for (itr = EventsManager::_sockets.begin(), next = itr;
         itr != EventsManager::_sockets.end(); itr = next) {
        ++next;
        if (itr->second.get_status() == fd_status::closed) {
            FD_CLR(itr->second.get_fd(), &EventsManager::_read_set);
            FD_CLR(itr->second.get_fd(), &EventsManager::_write_set);
            EventsManager::_sockets.erase(itr);
        }
    }
}
#endif
}  // namespace network
