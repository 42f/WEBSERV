#include "PollFd.hpp"

namespace network {
PollFd::PollFd(void) {}

PollFd::PollFd(std::vector<network::ServerSocket> s)
    : _timeout(-1), _capacity(0), _nb_socket(0), _nb_ready(0), _request_nb(0) {
    _fds = new struct pollfd[s.size()];
    _size = 0;
    for (std::vector<network::ServerSocket>::iterator it = s.begin();
         it != s.end(); it++) {
        _fds[_size].fd = it->get_id();
        _fds[_size].events = POLLIN;
        _sockets.push_back(Socket(it->get_id(), fd_status::is_listener));
        _size++;
    }
    _nb_socket = _size;
    _capacity = _size;
}

PollFd::~PollFd() {}

int PollFd::get_nb_ssocket(void) const { return _nb_socket; }
int PollFd::get_size(void) const { return _size; }
int PollFd::get_timeout(void) const { return _timeout; }
int PollFd::get_capacity(void) const { return _capacity; }
int PollFd::get_nb_ready(void) const { return _nb_ready; }
int PollFd::get_request_nb(void) const { return _request_nb; }
int PollFd::get_fd(int index) const { return _fds[index].fd; }
int PollFd::get_events(int index) const { return _fds[index].events; }
int PollFd::get_revents(int index) const { return _fds[index].revents; }

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
    struct pollfd *tmp;
    if (_size + 1 >= _capacity) {
        if (_capacity == 0) {
            _capacity = 1;
            tmp = new struct pollfd[_capacity];
        } else {
            _capacity *= 2;
            tmp = new struct pollfd[_capacity];
        }
        for (int i = 0; i < _size; i++) {
            tmp[i] = _fds[i];
        }
        if (_size > 0) {
            delete _fds;
        }
        _fds = tmp;
    }
    if (socket.get_fd() > 0) {
        _fds[_size].fd = socket.get_fd();
        _fds[_size].events = POLLIN | POLLOUT;
        _sockets.push_back(Socket(socket.get_fd(), fd_status::accepted));
        _size++;
    }
}

void PollFd::resize(void) {
    for (int i = _nb_socket; i < _size; i++) {
        if (_sockets[i].get_status() == fd_status::closed) {
            _sockets.erase(_sockets.begin() + i);
            for (int j = i; j < _size; j++) {
                _fds[j].fd = _fds[j + 1].fd;
            }
            _size--;
        }
    }
}
void PollFd::do_poll(void) { _nb_ready = poll(_fds, _size, _timeout); }

bool PollFd::is_acceptable(int i) {
    if (i > _size || i < 0) return (false);
    if (_fds[i].revents & POLLIN &&
        _sockets[i].get_status() == fd_status::is_listener) {
        return (true);
    }
    return (false);
}

bool PollFd::is_readable(int i) {
    if (i > _size || i < 0) return (false);
    if (_fds[i].revents & POLLIN &&
        _sockets[i].get_status() == fd_status::accepted) {
        return (true);
    }
    return (false);
}
bool PollFd::is_writable(int i) {
    if (i > _size || i < 0) return (false);
    if (_fds[i].revents == POLLOUT &&
        _sockets[i].get_status() ==
            fd_status::read) {  // && _socket[i].is_ready() == true)
        return (true);
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