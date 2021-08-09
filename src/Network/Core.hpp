#ifndef NETWORK_CORE_HPP
#define NETWORK_CORE_HPP

#include <errno.h>
#include <sys/select.h>  // select()
#include <sys/socket.h>
#include <unistd.h>  // close()

#include <iostream>
#include <vector>

#include "EventsManager.hpp"
#include "ServerSocket.hpp"
#include "ThreadPool.hpp"

namespace network {
void *ok(void *args);
class Core {
   public:
    Core(std::vector<network::ServerSocket> s, int _size_tpool);
    ~Core();

    void run_servers(void);
    void update_events(void);
    void check_sockets(void);
    void check_requests(void);
    void check_responses(void);

   private:
    Core(void);
    ThreadPool _tpool;
    int _nb_events;
};
}  // namespace network

std::ostream &operator<<(std::ostream &o, network::Core const &p);
void *ok(void *args);

#endif