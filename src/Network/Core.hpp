#ifndef NETWORK_CORE_HPP
#define NETWORK_CORE_HPP

#include <errno.h>
#include <sys/select.h>  // select()
#include <sys/socket.h>
#include <unistd.h>  // close()

#include <iostream>
#include <vector>

#include "EventManager.hpp"
#include "ServerSocket.hpp"

namespace network {
void *ok(void *args);
class Core {
   public:
    Core(std::vector<network::ServerSocket> s);
    ~Core();

    void run_servers(void);

   private:
    void update_events(void);
    void check_sockets(void);
    void check_requests(void);
    void check_responses(void);
    Core(void);
    int _nb_events;
};
}  // namespace network

std::ostream &operator<<(std::ostream &o, network::Core const &p);
void *ok(void *args);

#endif
