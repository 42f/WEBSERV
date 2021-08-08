#include "EventsManager.hpp"

namespace network {
EventsManager::EventsManager(std::vector<network::ServerSocket> s)
    : _timeout(0),
      _capacity(0),
      _nb_events(0),
      _nb_ssockets(0),
      _max_ssockets(0),
      _total_requests(0) {
    _monitor = new struct kevent[s.size()];
    _size = 0;
    _kq = kqueue();
    if (_kq < 0) perror("Kqueue");
    for (std::vector<network::ServerSocket>::iterator it = s.begin();
         it != s.end(); it++) {
        EV_SET(&_monitor[_size], it->get_id(), EVFILT_READ, EV_ADD | EV_ENABLE,
               0, 0, 0);
        _sockets.push_back(Socket(it->get_id(), fd_status::listener));
        _size++;
        if (it->get_id() > _max_ssockets) {
            _max_ssockets = it->get_id();
            std::cout << "Server socket was added on fd : " << it->get_id()
                      << std::endl;
        }
    }
    _nb_ssockets = _size;
    _capacity = _size;
}

EventsManager::~EventsManager() {
    delete[] _monitor;
    delete[] _events;
}

int EventsManager::get_kq(void) const { return _kq; }
int EventsManager::get_size(void) const { return _sockets.size(); }
int EventsManager::get_event_index(int fd) {
    for (int i = 0; i < _nb_events; i++) {
        if (_events[i].ident == static_cast<unsigned long>(fd)) return (i);
    }
    return (-1);
}
int EventsManager::get_event_fd(int index) { return _events[index].ident; }
int EventsManager::get_timeout(void) const { return _timeout; }
int EventsManager::get_capacity(void) const { return _capacity; }
int EventsManager::get_nb_events(void) const { return _nb_events; }
int EventsManager::get_nb_ssockets(void) const { return _nb_ssockets; }
int EventsManager::get_max_ssockets(void) const { return _max_ssockets; }
int EventsManager::get_total_requests(void) const { return _total_requests; }
struct kevent EventsManager::get_event(int index) const {
    return _events[index];
}

bool EventsManager::is_readable(int index) {
    // if (_sockets[index].has_events())
    // 	std::cout << "has event" << std::endl;
    // if (_sockets[index].get_flags() & EVFILT_READ)
    // 	std::cout << "has read flag" << std::endl;
    // if (_sockets[index].get_status() == fd_status::accepted)
    // 	std::cout << "is accepted" << std::endl;
    if (_sockets[index].has_events() &&
        _sockets[index].get_flags() & EVFILT_READ &&
        _sockets[index].get_status() == fd_status::accepted)
        return true;
    return (false);
}

bool EventsManager::is_writable(int index) const {
    // if (_sockets[index].has_events())
    // 	std::cout << "has event" << std::endl;
    // if (_sockets[index].get_flags() & EVFILT_WRITE)
    // 	std::cout << "has read flag" << std::endl;
    // if (_sockets[index].get_status() == fd_status::read)
    // 	std::cout << "is accepted" << std::endl;
    if (_sockets[index].has_events() &&
        _sockets[index].get_flags() & EVFILT_WRITE &&
        _sockets[index].get_status() == fd_status::read)
        return true;
    return (false);
}

bool EventsManager::is_acceptable(int index) const {
    if (_sockets[index].has_events() &&
        _sockets[index].get_flags() & EVFILT_READ &&
        _sockets[index].get_status() == fd_status::listener)
        return true;
    return false;
}

bool EventsManager::is_request(int index) const {
    if (index >= _nb_ssockets) return (true);
    return (false);
}

bool EventsManager::is_server(int index) const { return (!is_request(index)); }

void EventsManager::do_kevent(void) {
    for (unsigned long i = 0; i < _sockets.size(); i++) {
        if (_sockets[i].get_status() == fd_status::closed) {
            _sockets.erase(_sockets.begin() + i);
        }
    }
    _events = new struct kevent[_size];
    _nb_events = kevent(_kq, _monitor, _size, _events, _size, 0);
    if (_nb_events < 0) perror("kevent");
    for (int j = 0; j < _nb_events; j++) {
        for (unsigned long i = 0; i < _sockets.size(); i++) {
            if (static_cast<unsigned long>(_sockets[i].get_fd()) ==
                _events[j].ident) {
                _sockets[i].set_has_events(true);
                _sockets[i].set_flags(_events[j].flags);
            } else {
                _sockets[i].set_has_events(false);
                _sockets[i].set_flags(0);
            }
        }
    }
}

void EventsManager::add(int fd) {
    struct kevent *tmp;
    if (_size + 1 >= _capacity) {
        if (_capacity == 0) {
            _capacity = 1;
            tmp = new struct kevent[_capacity];
        } else {
            _capacity *= 2;
            tmp = new struct kevent[_capacity];
        }
        for (int i = 0; i < _size; i++) {
            tmp[i] = _monitor[i];
        }
        if (_size > 0) {
            delete _monitor;
        }
        _monitor = tmp;
    }
    if (fd > 0) {
        EV_SET(&_monitor[_size], fd, EVFILT_READ | EVFILT_WRITE,
               EV_ADD | EV_ENABLE, 0, 0, 0);
        _sockets.push_back(Socket(fd, fd_status::accepted));
        int enable = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            perror("Set");
        _size++;
        std::cerr << std::endl;
    } else {
        std::cerr << "Error: cannot add fd < 0" << std::endl;
    }
}

int EventsManager::accept_request(int index) {
    int tmp_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    tmp_fd = accept(_sockets[index].get_fd(), (struct sockaddr *)&client_addr,
                    &addr_len);
    if (tmp_fd < 0)
        perror("Accept");
    else {
        add(tmp_fd);
        _total_requests++;
        std::cout << _total_requests << std::endl;
    }
    return (0);
}

int EventsManager::recv_request(int index) {
    char buffer[4096];
    int ret;
    ret = recv(_sockets[index].get_fd(), buffer, 4096, 0);
    _sockets[index].set_status(fd_status::read);
    return (0);
}

int EventsManager::send_response(int index) {
    int ret = send(_sockets[index].get_fd(), "HTTP/1.1 OK 200\n\n500\n", 21, 0);
    if (ret > 0) {
        close(_sockets[index].get_fd());
        _sockets[index].set_status(fd_status::closed);
    }
    return (0);
}

void EventsManager::resize(void) {
    for (unsigned long i = 0; i < _sockets.size(); i++) {
        if (_sockets[i].get_status() == fd_status::closed) {
            _sockets.erase(_sockets.begin() + i);
            for (int j = i; j < _size; j++) {
                _monitor[j] = _monitor[j + 1];
            }
            _size--;
        }
    }
}
}  // namespace network

// while (i < _tpool.size()) {
//     if (_tpool[i].get_status() == thread_status::available) {
// if (_tpool[0].get_status() == thread_status::available) {
//     _tpool[0].set_status(thread_status::busy);
//     _fds.set_status(i, fd_status::closed);
//     // std::cout << "thread 0 : " << _fds.get_request_nb() <<
//     // std::endl;
//     _tpool[0].set_fd(_fds.get_fd(i));
//     _tpool[0].wake();
// } else if (_tpool[1].get_status() == thread_status::available) {
//     _tpool[1].set_status(thread_status::busy);
//     _fds.set_status(i, fd_status::closed);
//     // std::cout << "thread 1 : " << _fds.get_request_nb() <<
//     // std::endl;
//     _tpool[1].set_fd(_fds.get_fd(i));
//     _tpool[1].wake();
// } else if (_tpool[2].get_status() == thread_status::available) {
//     _tpool[2].set_status(thread_status::busy);
//     _fds.set_status(i, fd_status::closed);
//     // std::cout << "thread 2 : " << _fds.get_request_nb() <<
//     // std::endl;
//     _tpool[2].set_fd(_fds.get_fd(i));
//     _tpool[2].wake();
// } else if (_tpool[3].get_status() == thread_status::available) {
//     _tpool[3].set_status(thread_status::busy);
//     _fds.set_status(i, fd_status::closed);
//     // std::cout << "thread 3 : " << _fds.get_request_nb() <<
//     // std::endl;
//     _tpool[3].set_fd(_fds.get_fd(i));
//     _tpool[3].wake();
// } else if (_tpool[4].get_status() == thread_status::available) {
//     _tpool[4].set_status(thread_status::busy);
//     _fds.set_status(i, fd_status::closed);
//     // std::cout << "thread 4 : " << _fds.get_request_nb() <<
//     // std::endl;
//     _tpool[4].set_fd(_fds.get_fd(i));
//     _tpool[4].wake();
// }
//         break ;
//     }
//     i++;
// }