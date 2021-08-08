#include "Poll.hpp"

#include <stdio.h>
namespace network {

void *ok(void *args) {
    network::Thread *self = static_cast<network::Thread *>(args);
    while (1) {
        self->thread_sleep();  // sleeps here / wakes up here
        int ret = send(self->get_fd(), "HTTP/1.1 OK 200\n\n500\n", 21, 0);
        if (ret > 0) {
            // int oui = rand() % 10;
            // if (oui == 1)
            //     sleep(6);
            // usleep(1000);
            close(self->get_fd());
        }
        self->end_work();  // end sleeps here
    }
    return (NULL);
}

/*
 * Constructors and destructor
 */
Poll::Poll(std::vector<network::ServerSocket> s, int tpool_size)
    : _fds(PollFd(s)) {
    _tpool.create(tpool_size);
    _tpool.init(ok);
}
Poll::~Poll() {}

/*
 * Member functions
 */

void Poll::do_poll(void) { _fds.do_poll(); }

void Poll::check_sockets(std::vector<network::ServerSocket> s) {
    if (_fds.get_nb_ready() > 0) {
        for (int i = 0; i < _fds.get_nb_ssocket(); i++) {
            // if (_fds.is_acceptable(i)) {
            int tmp;
            tmp = s[i].do_accept();
            while (tmp > 0) {
                std::cout << tmp << std::endl;
                _fds.set_request_nb(_fds.get_request_nb() + 1);
                _fds.add(Socket(tmp, fd_status::accepted));
                _fds.set_nb_ready(_fds.get_nb_ready() - 1);
                tmp = s[i].do_accept();
            }
            // }
        }
    }  // if some sockets are ready
}

void Poll::check_requests(void) {
    if (_fds.get_nb_ready() > 0) {
        for (int i = _fds.get_nb_ssocket(); i < _fds.get_size(); i++) {
            if (_fds.is_readable(i)) {
                // std::cout << _fds.get_request_nb() << " is going to be read"
                // << std::endl;
                char buffer[4096];
                int ret;
                ret = recv(_fds.get_fd(i), buffer, 4096, 0);
                if (ret <= 0) {
                    _fds.set_status(i, fd_status::error);
                }
                _fds.set_status(i, fd_status::read);
                _fds.set_nb_ready(_fds.get_nb_ready() - 1);
            }
        }
    }
}

void Poll::send_response(void) {
    for (int i = _fds.get_nb_ssocket(); i < _fds.get_size(); i++) {
        if (_fds.is_writable(i)) {  // && response is ready)
            // std::cout << _fds.get_request_nb() << " is going to be written"
            // << std::endl; while (i < _tpool.size()) {
            //     if (_tpool[i].get_status() == thread_status::available) {
            if (_tpool[0].get_status() == thread_status::available) {
                _tpool[0].set_status(thread_status::busy);
                _fds.set_status(i, fd_status::closed);
                // std::cout << "thread 0 : " << _fds.get_request_nb() <<
                // std::endl;
                _tpool[0].set_fd(_fds.get_fd(i));
                _tpool[0].wake();
            } else if (_tpool[1].get_status() == thread_status::available) {
                _tpool[1].set_status(thread_status::busy);
                _fds.set_status(i, fd_status::closed);
                // std::cout << "thread 1 : " << _fds.get_request_nb() <<
                // std::endl;
                _tpool[1].set_fd(_fds.get_fd(i));
                _tpool[1].wake();
            } else if (_tpool[2].get_status() == thread_status::available) {
                _tpool[2].set_status(thread_status::busy);
                _fds.set_status(i, fd_status::closed);
                // std::cout << "thread 2 : " << _fds.get_request_nb() <<
                // std::endl;
                _tpool[2].set_fd(_fds.get_fd(i));
                _tpool[2].wake();
            } else if (_tpool[3].get_status() == thread_status::available) {
                _tpool[3].set_status(thread_status::busy);
                _fds.set_status(i, fd_status::closed);
                // std::cout << "thread 3 : " << _fds.get_request_nb() <<
                // std::endl;
                _tpool[3].set_fd(_fds.get_fd(i));
                _tpool[3].wake();
            } else if (_tpool[4].get_status() == thread_status::available) {
                _tpool[4].set_status(thread_status::busy);
                _fds.set_status(i, fd_status::closed);
                // std::cout << "thread 4 : " << _fds.get_request_nb() <<
                // std::endl;
                _tpool[4].set_fd(_fds.get_fd(i));
                _tpool[4].wake();
            }
            //         break ;
            //     }
            //     i++;
            // }
        }
    }
}

void Poll::run_servers(std::vector<network::ServerSocket> s) {
    for (;;) {
        _fds.do_poll();
        check_sockets(s);
        check_requests();
        send_response();
        _fds.resize();
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