#include "PollFd.hpp"

namespace network {
PollFd::PollFd(void) {}

PollFd::PollFd(std::vector<network::ServerSocket> s)
    : _timeout(-1), _capacity(0), _nb_socket(0), _nb_ready(0), _request_nb(0) {
    _monitor = new struct kevent[s.size()];
    _size = 0;
    _max_socket = 0;
    _kq = kqueue();
    if (_kq < 0) perror("Kqueue");
    for (std::vector<network::ServerSocket>::iterator it = s.begin();
         it != s.end(); it++) {
        EV_SET(&_monitor[_size], it->get_id(), EVFILT_READ, EV_ADD | EV_ENABLE,
               0, 0, 0);
        _sockets.push_back(Socket(it->get_id(), fd_status::is_listener));
        _size++;
        if (it->get_id() > _max_socket) {
            _max_socket = it->get_id();
            std::cout << "Server socket was added on fd : " << it->get_id()
                      << std::endl;
        }
    }
    _nb_socket = _size;
    _capacity = _size;
}

PollFd::~PollFd() {}

int PollFd::get_nb_ssocket(void) const { return _nb_socket; }
int PollFd::get_max_ssocket(void) const { return _max_socket; }
int PollFd::get_size(void) const { return _size; }
int PollFd::get_timeout(void) const { return _timeout; }
int PollFd::get_capacity(void) const { return _capacity; }
int PollFd::get_nb_ready(void) const { return _nb_ready; }
int PollFd::get_request_nb(void) const { return _request_nb; }
int PollFd::get_fd(int index) const { return _monitor[index].ident; }
struct kevent PollFd::get_events(int index) const {
    return _events[index];
}
// int PollFd::get_revents(int index) const { return _monitor[index].revents; }

void PollFd::set_nb_ssocket(int value) { _nb_socket = value; }
void PollFd::set_size(int value) { _size = value; }
void PollFd::set_timeout(int value) { _timeout = value; }
void PollFd::set_capacity(int value) { _capacity = value; }
void PollFd::set_nb_ready(int value) { _nb_ready = value; }
void PollFd::set_request_nb(int value) { _request_nb = value; }
void PollFd::set_status(int index, fd_status::status status) {
    _sockets[index].set_status(status);
}

void PollFd::add(network::Socket socket) {
    std::cout << "add has been called" << std::endl;
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
    if (socket.get_fd() > 0) {
        EV_SET(&_monitor[_size], socket.get_fd(), EVFILT_READ | EVFILT_WRITE,
               EV_ADD | EV_ENABLE, 0, 0, 0);
        _sockets.push_back(Socket(socket.get_fd(), fd_status::accepted));
        _size++;
    } else {
        std::cerr << "fd < 0" << std::endl;
    }
}

void PollFd::resize(void) {
    for (int i = _nb_socket; i < _size; i++) {
        if (_sockets[i].get_status() == fd_status::closed) {
            _sockets.erase(_sockets.begin() + i);
            for (int j = i; j < _size; j++) {
                _monitor[j] = _monitor[j + 1];
            }
            _size--;
        }
    }
}
void PollFd::do_poll(void) {
    // std::cout << "before poll ";
    // for (int i = 0; i < _size; i++) {
    //     std::cout << " [" << _monitor[i].ident << "] ";
    // }
    // std::cout << "s: " << _size << " c: " << _capacity << " n: " <<
    // _request_nb
    //           << std::endl;
    _events = new struct kevent[_size];
    _nb_ready = kevent(_kq, _monitor, _size, _events, _size, 0);
    if (_nb_ready < 0) {
        perror("Kevent");
    } else {
        std::cout << std::endl << _nb_ready << " events occured" << std::endl;
        for (int i = 0; i < _nb_ready; i++) {
            if (_events[i].flags & EVFILT_READ)
                std::cout << "Can READ on fd : " << _events[i].ident
                          << std::endl;
            if (_events[i].flags & EVFILT_WRITE)
                std::cout << "Can WRITE on fd : " << _events[i].ident
                          << std::endl;
        }
    }
    // std::cout << "Polling done : " << _nb_ready << std::endl;
}

bool PollFd::is_acceptable(int i) {
    if (i > _size || i < 0) return (false);
    if (_nb_ready > 0 && _events[i].flags & EVFILT_READ) {
        for (int j = 0; j < _size; j++) {
            if (static_cast<unsigned long>(_sockets[j].get_fd()) == _events[i].ident &&
                _sockets[i].get_status() == fd_status::is_listener)
                return true;
        }
    }
    return (false);
}

bool PollFd::is_readable(int i) {
    if (i > _size || i < 0) return (false);
    if (_nb_ready > 0 && _events[i].flags & EVFILT_READ) {
        for (int j = 0; j < _size; j++) {
            if (static_cast<unsigned long>(_sockets[j].get_fd()) == _events[i].ident &&
                _sockets[j].get_status() == fd_status::accepted)
                return true;
        }
    }
    return (false);
}
bool PollFd::is_writable(int i) {
    std::cout << "yead " << i << std::endl;
    if (i > _size || i < 0) return (false);
    if (_nb_ready > 0 && _events[i].flags & EVFILT_WRITE) {
        for (int j = 0; j < _size; j++) {
            if (static_cast<unsigned long>(_sockets[j].get_fd()) == _events[i].ident &&
                _sockets[j].get_status() == fd_status::read)
                return true;
        }
    }
    return (false);
}

void PollFd::close(int fd) {
    for (int i = 0; i < _size; i++) {
        if (_sockets[i].get_fd() == fd) {
            _sockets[i].set_status(fd_status::closed);
        }
    }
}
}  // namespace network