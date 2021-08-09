#include "Core.hpp"

namespace network {

void *ok(void *args) {
    Thread *self = static_cast<Thread *>(args);
    for (;;) {
        self->thread_sleep();
        std::cout << "in thread before fn" << std::endl;
        // if (self->get_number() == 0) sleep(4);
        EventsManager::send_response(self->get_socket_index());
        std::cout << "in thread after fn" << std::endl;
        // do something
        self->end_work();
    }

    return (NULL);
}

Core::Core(std::vector<network::ServerSocket> s, int tpool_size)
    : _nb_events(0) {
    EventsManager::init(s);
    _tpool.create(tpool_size);
    _tpool.init(ok);
}
Core::~Core() {}

void Core::update_events(void) {
    EventsManager::do_kevent();
    if (EventsManager::get_nb_events() < 0) {
        std::cerr << "Error Kevent : system failure" << std::endl;
    } else {
        _nb_events = EventsManager::get_nb_events();
    }
}

void Core::check_sockets(void) {
    if (EventsManager::get_nb_events() > 0) {
        for (int i = 0; i < EventsManager::get_size(); i++) {
            if (EventsManager::is_acceptable(i)) {
                std::cout << "before accepting" << std::endl;
                EventsManager::accept_request(i);
                std::cout << "after accepting" << std::endl;
            }
        }
    }
}

void Core::check_requests(void) {
    if (EventsManager::get_nb_events() > 0) {
        for (int i = 0; i < EventsManager::get_size(); i++) {
            if (EventsManager::is_readable(i)) {
                std::cout << "i = " << i << " " << std::boolalpha << EventsManager::is_readable(i) << std::endl;
                std::cout << "before reading" << std::endl;
                EventsManager::recv_request(i);
                std::cout << "after reading" << std::endl;
            }
        }
    }
}

void Core::check_responses(void) {
    if (EventsManager::get_nb_events() > 0) {
        for (int i = 0; i < EventsManager::get_size(); i++) {
            if (EventsManager::is_writable(i)) {
                std::cout << "before sending" << std::endl;
                if (_tpool[0].get_status() == thread_status::available) {
                    _tpool[0].set_status(thread_status::busy);
                    _tpool[0].set_number(0);
                    std::cout
                        << "thread 0 : " << EventsManager::get_total_requests()
                        << std::endl;
                    _tpool[0].set_socket_index(i);
                    _tpool[0].wake();
                } else if (_tpool[1].get_status() == thread_status::available) {
                    _tpool[1].set_status(thread_status::busy);
                    _tpool[1].set_number(1);
                    std::cout
                        << "thread 1 : " << EventsManager::get_total_requests()
                        << std::endl;
                    _tpool[1].set_socket_index(i);
                    _tpool[1].wake();
                }
                std::cout << "after sending" << std::endl;
            }
        }
    }
}

void Core::run_servers(void) {
    for (;;) {
        update_events();
        check_sockets();
        check_requests();
        check_responses();
        EventsManager::resize();
    }
}

}  // namespace network
