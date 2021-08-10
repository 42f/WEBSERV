#include "EventsManager.hpp"

namespace network {
int EventsManager::_kq = 0;
int EventsManager::_timeout = 0;
int EventsManager::_nb_events = 0;
int EventsManager::_max_ssocket = 0;
int EventsManager::_total_requests = 0;
struct kevent *EventsManager::_monitor;
struct kevent *EventsManager::_events;
std::vector<Socket> EventsManager::_sockets;

/*
 *	Initializes the vector of sockets and the kevent monitor list with the
 *listening sockets Also, sets the maximum listening socket number
 */

void EventsManager::init(std::vector<network::ServerSocket> s) {
    EventsManager::_monitor = new struct kevent[s.size()];
    EventsManager::_events = new struct kevent[s.size()];

    if ((EventsManager::_kq = kqueue()) < 0) perror("Kqueue");

    int i = 0;
    for (std::vector<network::ServerSocket>::iterator it = s.begin();
         it != s.end(); it++) {
        EV_SET(&EventsManager::_monitor[i], it->get_id(), EVFILT_READ,
               EV_ADD | EV_ENABLE, 0, 0, 0);

        EventsManager::_sockets.push_back(
            Socket(it->get_id(), fd_status::listener));

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

bool EventsManager::is_readable(int index) {
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_READ &&
        EventsManager::_sockets[index].get_status() == fd_status::accepted)
        return true;
    return (false);
}

bool EventsManager::is_writable(int index) {
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_WRITE &&
        EventsManager::_sockets[index].get_status() == fd_status::read) {
        // _sockets[index].set_status(fd_status::written);
        return true;
    }
    return (false);
}

bool EventsManager::is_acceptable(int index) {
    if (EventsManager::_sockets[index].has_events() &&
        EventsManager::_sockets[index].get_flags() & EVFILT_READ &&
        EventsManager::_sockets[index].get_status() == fd_status::listener)
        return true;
    return false;
}

void EventsManager::do_kevent(void) {
    delete[] EventsManager::_events;
    EventsManager::_events = new struct kevent[_sockets.size()];

    EventsManager::_nb_events =
        kevent(EventsManager::_kq, EventsManager::_monitor, _sockets.size(),
               EventsManager::_events, _sockets.size(), 0);
    if (EventsManager::_nb_events < 0) perror("kevent");

    for (int j = 0; j < EventsManager::_nb_events; j++) {
        for (unsigned long i = 0; i < EventsManager::_sockets.size(); i++) {
            if (static_cast<unsigned long>(
                    EventsManager::_sockets[i].get_fd()) ==
                EventsManager::_events[j].ident) {
                EventsManager::_sockets[i].set_has_events(true);
                EventsManager::_sockets[i].set_flags(
                    EventsManager::_events[j].flags);
                break;
            } else {
                EventsManager::_sockets[i].set_has_events(false);
                EventsManager::_sockets[i].set_flags(0);
            }
        }
    }
}

void EventsManager::add(int fd) {
    if (fd > 0) {
        EventsManager::_sockets.push_back(Socket(fd, fd_status::accepted));
        delete[] _monitor;
        _monitor = new struct kevent[_sockets.size()];
        for (unsigned long i = 0; i < _sockets.size(); i++) {
            EV_SET(&EventsManager::_monitor[i], _sockets[i].get_fd(),
                   EVFILT_READ | EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
            std::cout << _monitor[i].ident << " | " << _sockets[i].get_fd()
                      << " has been added" << std::endl;
        }
    } else {
        std::cerr << "Error: cannot add fd < 0" << std::endl;
    }
}

int EventsManager::accept_request(int index) {
    int tmp_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if (index >= static_cast<int>(_sockets.size()) || index < 0) return (-1);

    tmp_fd = accept(EventsManager::_sockets[index].get_fd(),
                    (struct sockaddr *)&client_addr, &addr_len);
    std::cout << tmp_fd << " has been accepted" << std::endl;
    if (tmp_fd < 0)
        perror("Accept");
    else {
        add(tmp_fd);
        EventsManager::_total_requests++;
    }
    return (tmp_fd);
}

int EventsManager::recv_request(int index) {
    char buffer[4096];
    int ret;

    if (index >= static_cast<int>(_sockets.size()) || index < 0) return (-1);
    std::cout << "receving " << _sockets[index].get_fd() << std::endl;

    ret = recv(EventsManager::_sockets[index].get_fd(), buffer, 4096, 0);
    if (ret == 0) {
        std::cerr << "Connection closed by client" << std::endl;
        close(_sockets[index].get_fd());
        EventsManager::_sockets[index].set_status(fd_status::closed);
        return (-1);
    } else {
        EventsManager::_sockets[index].manage_raw_request(buffer, ret);
        EventsManager::_sockets[index].set_status(fd_status::read);
        // std::cout << "flag read: " << std::boolalpha <<
        // (EventsManager::_sockets[index].get_status() == fd_status::read) << "
        // fd: " << EventsManager::_sockets[index].get_fd() << std::endl;
    }
    return (0);
}

int EventsManager::send_response(int index) {
    std::ostringstream buffer;

    if (index >= static_cast<int>(_sockets.size()) || index < 0) return (-1);
    std::cout << "sending " << _sockets[index].get_fd() << std::endl;

    buffer << EventsManager::_sockets[index].get_response();
    // std::cout << "sending " << _sockets[index].get_fd() << std::endl;

    if (_sockets[index].get_fd() == 6) sleep(4);

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
            for (unsigned long j = i; j < EventsManager::_sockets.size(); j++) {
                EventsManager::_monitor[j] = EventsManager::_monitor[j + 1];
            }
            for (unsigned long k = 0; k < _sockets.size(); k++) {
                std::cout << "[" << _sockets[k].get_fd() << "] ";
            }
            std::cout << std::endl;
        }
    }
}
}  // namespace network
