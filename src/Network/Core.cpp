#include "Core.hpp"

namespace network {

Core::Core(std::vector<network::ServerSocket> s) : _nb_events(0) {
    EventManager::init(s);
}
Core::~Core() {}

void Core::update_events(void) {
    EventManager::do_select();
    if (EventManager::get_nb_events() < 0) {
        std::cerr << "Error Select : system failure" << std::endl;
    } else {
        _nb_events = EventManager::get_nb_events();
    }
}

void Core::check_sockets(void) {
    if (EventManager::get_nb_events() > 0) {
        EventManager::accept_request(0);
    }
}

void Core::check_requests(void) { EventManager::recv_request(0); }

void Core::check_responses(void) {
    EventManager::send_response(0);
    std::cout << "before resize" << std::endl;
    EventManager::resize();
    std::cout << "after resize" << std::endl;
}

void Core::run_servers(void) {
    for (;;) {
        std::cout << "update" << std::endl;
        update_events();
        std::cout << "check sockets" << std::endl;
        check_sockets();
        std::cout << "check requests" << std::endl;
        check_requests();
        std::cout << "check responses" << std::endl;
        check_responses();
        std::cout << "done loop" << std::endl;
    }
}

}  // namespace network
