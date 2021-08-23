#ifndef NETWORK_EVENTMANAGER_HPP
#define NETWORK_EVENTMANAGER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>   // | kqueue(), struct kevent, kevent()
#include <sys/types.h>  // |
#include <unistd.h>     // close()

#include <list>  // stl list

#include "HTTP/RequestHandler.hpp"
#include "ServerSocket.hpp"  // network::ServerSockets
#include "Socket.hpp"        // network::Sockets, fd_status::status

namespace network {

/*
 * An EventManager is used to manage all requests on a given vector of listening
 * socket
 */

class EventManager {
 public:
  ~EventManager();

  static void init(std::vector<network::ServerSocket> s);

  static void resize(void);
  static void do_select(void);
  static void accept_request(int index);
  static void recv_request(int index);
  static void send_response(int index);

  static int get_nb_events(void);
  static int get_total_requests(void);
  static unsigned long get_size(void);

 private:
  static int _kq;
  static int _max_fd;
  static int _timeout;
  static int _nb_events;
  static int _max_ssocket;
  static int _total_requests;
  static fd_set _read_set;
  static fd_set _write_set;
  static std::list<Socket*> _sockets;

  static void add(int fd, int port, struct sockaddr_in client_addr);
};
}  // namespace network

#endif