#include "Poll.hpp"

namespace network {

void *ok(void *args) {
    struct pollfd *fd = static_cast<struct pollfd *>(args);

    sleep(5);
    close(fd->fd);
    fd->fd = -1;
    return (NULL);
}

/*
 * Constructors and destructor
 */
Poll::Poll(std::vector<network::ServerSocket> s) : _fds(PollFd(s)) {}
Poll::~Poll() {}

/*
 * Member functions
 */

void Poll::do_poll(void) { _fds.do_poll(); }

void Poll::check_sockets(std::vector<network::ServerSocket> s) {
    if (_fds.get_nb_ready() > 0) {
        for (int i = 0; i < _fds.get_nb_ssocket(); i++) {
            if (_fds.is_acceptable(i)) {
                int tmp = 0;
                tmp = s[i].do_accept();
                // _fds._request_nb++;
                _fds.add(Socket(tmp, fd_status::accepted));
                // std::cout << _request_nb << " request accepted [" << tmp <<
                // "]"
                // 		  << std::endl;
                _fds.set_nb_ready(_fds.get_nb_ready() - 1);
            }
        }
    }  // if some sockets are ready
}

void Poll::check_requests(void) {
    if (_fds.get_nb_ready() > 0) {
        for (int i = _fds.get_nb_ssocket(); i < _fds.get_size(); i++) {
            if (_fds.is_readable(i)) {
                char buffer[4096];
                int ret;
                // std::cout << _request_nb << " reading request" << std::endl;
                ret = recv(_fds.get_fd(i), buffer, 4096, 0);
                if (ret <= 0) {
                    _fds.set_status(i, fd_status::error);
                }
                // _ready--;
                _fds.set_status(i, fd_status::read);
                _fds.set_nb_ready(_fds.get_nb_ready() - 1);
            }
        }
    }
}

void Poll::send_response(void) {
    for (int i = _fds.get_nb_ssocket(); i < _fds.get_size(); i++) {
        if (_fds.is_writable(i)) {  // && response is ready)
            int ret;
            // std::cout << _request_nb << " writing on request" << std::endl
            //           << std::endl;
            ret = send(_fds.get_fd(i), "HTTP/1.1 OK 200\n\n500\n", 21, 0);
            if (ret > 0) {
                close(_fds.get_fd(i));
                _fds.set_status(i, fd_status::closed);
            }
            // pthread_t t;
            // pthread_create(&t, NULL, ok, &(_fds.get_fd(i)));
        }
    }
    _fds.resize();
}

void Poll::run_servers(std::vector<network::ServerSocket> s) {
    for (;;) {
        _fds.do_poll();
        check_sockets(s);
        check_requests();
        send_response();
    }
}

std::ostream &operator<<(std::ostream &o, network::Poll const &p) {
    (void)p;
    // std::cout << "Listening on " << p.get_size() << " socket(s) : ";
    // for (int i = 0; i < p.get_size(); i++) {
    //     o << "[" << p.get_fds()[i].fd << "] ";
    // }
    return (o);
}
}  // namespace network