#include "Core.hpp"

namespace network {
Core::Core(std::vector<network::ServerSocket> s, int tpool_size)
    : _e_manager(EventsManager(s)), _nb_events(0) {
    _tpool.create(tpool_size);
    // _tpool.init(ok);
}
Core::~Core() {}

void Core::update_events(void) {
    _e_manager.do_kevent();
    if (_e_manager.get_nb_events() < 0) {
        std::cerr << "Error Kevent : system failure" << std::endl;
    } else {
        _nb_events = _e_manager.get_nb_events();
    }
}

void Core::check_sockets(void) {
    if (_e_manager.get_nb_events() > 0) {
        for (int i = 0; i < _e_manager.get_size(); i++) {
            if (_e_manager.is_acceptable(i)) {
                // std::cout << "before accepting" << std::endl;
                _e_manager.accept_request(i);
                // std::cout << "after accepting" << std::endl;
            }
        }
    }
}

void Core::check_requests(void) {
    if (_e_manager.get_nb_events() > 0) {
        for (int i = 0; i < _e_manager.get_size(); i++) {
            if (_e_manager.is_readable(i)) {
                // std::cout << "before reading" << std::endl;
                _e_manager.recv_request(i);
                // std::cout << "after reading" << std::endl;
            }
        }
    }
}

void Core::check_responses(void) {
    if (_e_manager.get_nb_events() > 0) {
        for (int i = 0; i < _e_manager.get_size(); i++) {
            if (_e_manager.is_writable(i)) {
                // std::cout << "before sending" << std::endl;
                _e_manager.send_response(i);
                // std::cout << "after sending" << std::endl;
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
        _e_manager.resize();
    }
}

}  // namespace network
