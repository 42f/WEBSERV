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
  if (EventManager::get_nb_events() > 0 &&
      EventManager::get_sockets().size() < DEFAULT_MAX_OPEN_FD) {
    EventManager::accept_request();
  }
}

void Core::check_requests(void) { EventManager::recv_request(); }

void Core::check_responses(void) {
  EventManager::send_response();
  EventManager::resize();
}

void Core::run_servers(void) {
  for (;;) {
    update_events();
    check_sockets();
    check_requests();
    check_responses();
  }
}

}  // namespace network
