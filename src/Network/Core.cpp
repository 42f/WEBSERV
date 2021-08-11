#include "Core.hpp"

namespace network {

Core::Core(std::vector<network::ServerSocket> s)
    : _nb_events(0) {
    EventManager::init(s);
}
Core::~Core() {}

void Core::update_events(void) {
    EventManager::do_kevent();
    if (EventManager::get_nb_events() < 0) {
        std::cerr << "Error Kevent : system failure" << std::endl;
    } else {
        _nb_events = EventManager::get_nb_events();
    }
}

void Core::check_sockets(void) {
    if (EventManager::get_nb_events() > 0) {
#if POLL_FN == KQUEUE
        for (int i = 0; i < EventManager::get_nb_events(); i++) {
            int fd = EventManager::get_event_struct(i).ident;
            if (EventManager::is_acceptable(fd)) {
                EventManager::accept_request(fd);
            }
        }
#elif POLL_FN == SELECT
        EventManager::accept_request(0);
#endif
    }
}

void Core::check_requests(void) {
#if POLL_FN == KQUEUE

    if (EventManager::get_nb_events() > 0) {
        for (int i = 0; i < EventManager::get_nb_events(); i++) {
            int fd = EventManager::get_event_struct(i).ident;
            if (EventManager::is_readable(fd)) {
                EventManager::recv_request(fd);
            }
        }
    }
#elif POLL_FN == SELECT
    EventManager::recv_request(0);
#endif
}

void Core::check_responses(void) {
#if POLL_FN == KQUEUE
    for (int i = 0; i < EventManager::get_nb_events(); i++) {
        int fd = EventManager::get_event_struct(i).ident;
        if (EventManager::is_writable(fd)) {
            EventManager::send_response(fd);
        }
    }
#elif POLL_FN == SELECT
    EventManager::send_response(0);
#endif
    EventManager::resize();
}

void Core::run_servers(void) {
    for (;;) {
        // std::cout << "update" << std::endl;
        update_events();
        // std::cout << "check sockets" << std::endl;
        check_sockets();
        // std::cout << "check requests" << std::endl;
        check_requests();
        // std::cout << "check responses" << std::endl;
        check_responses();
        // std::cout << "done loop" << std::endl;
    }
}

}  // namespace network
