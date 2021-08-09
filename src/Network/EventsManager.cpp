#include "EventsManager.hpp"

namespace network {
int EventsManager::_kq = 0;
int EventsManager::_size = 0;
int EventsManager::_timeout = 0;
int EventsManager::_capacity = 0;
int EventsManager::_nb_events = 0;
int EventsManager::_nb_ssockets = 0;
int EventsManager::_max_ssockets = 0;
int EventsManager::_total_requests = 0;
struct kevent *EventsManager::_monitor;
struct kevent *EventsManager::_events;
std::vector<Socket> EventsManager::_sockets;

void EventsManager::init(std::vector<network::ServerSocket> s) {
    EventsManager::_monitor = new struct kevent[s.size()];
    EventsManager::_kq = kqueue();
    if (EventsManager::_kq < 0) perror("Kqueue");
    for (std::vector<network::ServerSocket>::iterator it = s.begin();
         it != s.end(); it++) {
        EV_SET(&EventsManager::_monitor[EventsManager::_size], it->get_id(),
               EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
        EventsManager::_sockets.push_back(
            Socket(it->get_id(), fd_status::listener));
        EventsManager::_size++;
        if (it->get_id() > EventsManager::_max_ssockets) {
            EventsManager::_max_ssockets = it->get_id();
            std::cout << "Server socket was added on fd : " << it->get_id()
                      << std::endl;
        }
    }
    EventsManager::_nb_ssockets = EventsManager::_size;
    EventsManager::_capacity = EventsManager::_size;
}

EventsManager::~EventsManager() {
    delete[] EventsManager::_monitor;
    delete[] EventsManager::_events;
}

int EventsManager::get_kq(void) { return EventsManager::_kq; }
int EventsManager::get_size(void) { return EventsManager::_sockets.size(); }
int EventsManager::get_event_index(int fd) {
    for (int i = 0; i < EventsManager::_nb_events; i++) {
        if (EventsManager::_events[i].ident == static_cast<unsigned long>(fd))
            return (i);
    }
    return (-1);
}
int EventsManager::get_event_fd(int index) {
    return EventsManager::_events[index].ident;
}
int EventsManager::get_timeout(void) { return EventsManager::_timeout; }
int EventsManager::get_capacity(void) { return EventsManager::_capacity; }
int EventsManager::get_nb_events(void) { return EventsManager::_nb_events; }
int EventsManager::get_nb_ssockets(void) { return EventsManager::_nb_ssockets; }
int EventsManager::get_max_ssockets(void) {
    return EventsManager::_max_ssockets;
}
int EventsManager::get_total_requests(void) {
    return EventsManager::_total_requests;
}
struct kevent EventsManager::get_event(int index) {
    return EventsManager::_events[index];
}

bool EventsManager::is_readable(int index) {
    // if (EventsManager::_sockets[index].has_events())
    // 	std::cout << "has event" << std::endl;
    // if (EventsManager::_sockets[index].get_flags() & EVFILT_READ)
    // 	std::cout << "has read flag" << std::endl;
    // if (EventsManager::_sockets[index].get_status() == fd_status::accepted)
    // 	std::cout << "is accepted" << std::endl;
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_READ &&
        EventsManager::_sockets[index].get_status() == fd_status::accepted)
        return true;
    return (false);
}

bool EventsManager::is_writable(int index) {
    // if (EventsManager::_sockets[index].has_events())
    // 	std::cout << "has event" << std::endl;
    // if (EventsManager::_sockets[index].get_flags() & EVFILT_WRITE)
    // 	std::cout << "has read flag" << std::endl;
    // if (EventsManager::_sockets[index].get_status() == fd_status::read)
    // 	std::cout << "is accepted" << std::endl;
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_WRITE &&
        EventsManager::_sockets[index].get_status() == fd_status::read)
        return true;
    return (false);
}

bool EventsManager::is_acceptable(int index) {
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_READ &&
        EventsManager::_sockets[index].get_status() == fd_status::listener)
        return true;
    return false;
}

bool EventsManager::is_request(int index) {
    if (index >= EventsManager::_nb_ssockets) return (true);
    return (false);
}

bool EventsManager::is_server(int index) { return (!is_request(index)); }

void EventsManager::do_kevent(void) {
    for (unsigned long i = 0; i < EventsManager::_sockets.size(); i++) {
        if (EventsManager::_sockets[i].get_status() == fd_status::closed) {
            EventsManager::_sockets.erase(EventsManager::_sockets.begin() + i);
        }
    }
    delete[] EventsManager::_events;
    EventsManager::_events = new struct kevent[EventsManager::_size];
    EventsManager::_nb_events = kevent(
        EventsManager::_kq, EventsManager::_monitor, EventsManager::_size,
        EventsManager::_events, EventsManager::_size, 0);
    if (EventsManager::_nb_events < 0) perror("kevent");
    for (int j = 0; j < EventsManager::_nb_events; j++) {
        for (unsigned long i = 0; i < EventsManager::_sockets.size(); i++) {
            if (static_cast<unsigned long>(
                    EventsManager::_sockets[i].get_fd()) ==
                EventsManager::_events[j].ident) {
                EventsManager::_sockets[i].set_has_events(true);
                EventsManager::_sockets[i].set_flags(
                    EventsManager::_events[j].flags);
            } else {
                EventsManager::_sockets[i].set_has_events(false);
                EventsManager::_sockets[i].set_flags(0);
            }
        }
    }
}

void EventsManager::add(int fd) {
    struct kevent *tmp;
    if (EventsManager::_size + 1 >= EventsManager::_capacity) {
        if (EventsManager::_capacity == 0) {
            EventsManager::_capacity = 1;
            tmp = new struct kevent[EventsManager::_capacity];
        } else {
            EventsManager::_capacity *= 2;
            tmp = new struct kevent[EventsManager::_capacity];
        }
        for (int i = 0; i < EventsManager::_size; i++) {
            tmp[i] = EventsManager::_monitor[i];
        }
        delete[] EventsManager::_monitor;
        EventsManager::_monitor = tmp;
    }
    if (fd > 0) {
        EV_SET(&EventsManager::_monitor[EventsManager::_size], fd,
               EVFILT_READ | EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
        EventsManager::_sockets.push_back(Socket(fd, fd_status::accepted));
        int enable = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            perror("Set");
        EventsManager::_size++;
    } else {
        std::cerr << "Error: cannot add fd < 0" << std::endl;
    }
}

int EventsManager::accept_request(int index) {
    int tmp_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    tmp_fd = accept(EventsManager::_sockets[index].get_fd(),
                    (struct sockaddr *)&client_addr, &addr_len);
    if (tmp_fd < 0)
        perror("Accept");
    else {
        add(tmp_fd);
        EventsManager::_total_requests++;
        std::cout << EventsManager::_total_requests << std::endl;
    }
    return (0);
}

int EventsManager::recv_request(int index) {
    char buffer[4096];
    int ret;
    std::cout << "fd : " << _sockets[index].get_fd() << std::endl;
    ret = recv(EventsManager::_sockets[index].get_fd(), buffer, 4096, 0);
    std::cout << "ret: " << ret << std::endl;
    EventsManager::_sockets[index].manage_raw_request(buffer, ret);
    EventsManager::_sockets[index].set_status(fd_status::read);
    return (0);
}

int EventsManager::send_response(int index) {
    std::ostringstream buffer;
    // buffer << "ok";
    buffer << EventsManager::_sockets[index].get_response();
    // int ret = send(EventsManager::_sockets[index].get_fd(), "HTTP/1.1 OK
    // 200\n\n", 20, 0);
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
    for (unsigned long i = 0; i < EventsManager::_sockets.size(); i++) {
        if (EventsManager::_sockets[i].get_status() == fd_status::closed) {
            EventsManager::_sockets.erase(EventsManager::_sockets.begin() + i);
            for (int j = i; j < EventsManager::_size; j++) {
                EventsManager::_monitor[j] = EventsManager::_monitor[j + 1];
            }
            EventsManager::_size--;
        }
    }
}
}  // namespace network
