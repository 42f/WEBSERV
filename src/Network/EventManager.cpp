#include "EventManager.hpp"

namespace network {
int EventManager::_kq = 0;
int EventManager::_timeout = 0;
int EventManager::_nb_events = 0;
int EventManager::_max_ssocket = 0;
int EventManager::_total_requests = 0;
fd_set EventManager::_read_set;
fd_set EventManager::_write_set;
int EventManager::_max_fd = 0;
std::vector<Socket> EventManager::_sockets;

/***************************************************
    Getters
***************************************************/

/*
 *	init() initializes the vector of active sockets and the first select
 *  read & write sets with the listening sockets. Also, init() sets the maximum
 *  listening socket number
 */

void EventManager::init(std::vector<network::ServerSocket> s) {
  int i = 0;
  for (std::vector<network::ServerSocket>::iterator it = s.begin();
       it != s.end(); it++) {
    FD_SET(it->get_id(), &EventManager::_read_set);
    EventManager::_sockets.push_back(
        Socket(it->get_id(), it->get_port(), fd_status::listener));

    if (it->get_id() > EventManager::_max_ssocket)
      EventManager::_max_ssocket = it->get_id();
    i++;
  }
  EventManager::_max_fd = EventManager::_max_ssocket;
}

EventManager::~EventManager() {}

/***************************************************
    Getters
***************************************************/

unsigned long EventManager::get_size(void) {
  return EventManager::_sockets.size();
}

int EventManager::get_nb_events(void) { return EventManager::_nb_events; }

int EventManager::get_total_requests(void) { return _total_requests; }

Socket& EventManager::get_socket(int index) {
  if (index < 0 || index >= static_cast<int>(_sockets.size()))
    throw(std::exception());
  return _sockets[index];
}

/***************************************************
    Functions
***************************************************/

/*
 *  do_select() clears select write & read sets, populate them with the file
 *  descriptors from the active sockets vector and calls the select() function
 * on all active sockets (listeners or not)
 */

void EventManager::do_select(void) {
  struct timeval tv = {1, 0};
  FD_ZERO(&EventManager::_read_set);
  FD_ZERO(&EventManager::_write_set);

  std::vector<Socket>::iterator itr;
  for (itr = EventManager::_sockets.begin();
       itr != EventManager::_sockets.end(); ++itr) {
    if (itr->get_fd() > _max_ssocket) {
      FD_SET(itr->get_fd(), &EventManager::_read_set);
      FD_SET(itr->get_fd(), &EventManager::_write_set);
    } else
      FD_SET(itr->get_fd(), &EventManager::_read_set);
  }
  EventManager::_nb_events =
      select(EventManager::_max_fd + 1, &EventManager::_read_set,
             &EventManager::_write_set, NULL, &tv);
  if (EventManager::_nb_events < 0) {
    perror("select");
    std::cerr << EventManager::_max_fd + 1 << std::endl;
  }
}

/*
 *  add creates a socket giving it an fd and a port, pushes it in the Active
 *  Socket vector and adds the fd into select read & write sets
 *
 *  add() only adds non-listening soccket
 */

void EventManager::add(int fd, int port) {
  if (fd > 0) {
    if (fd > EventManager::_max_fd) EventManager::_max_fd = fd;
    EventManager::_sockets.push_back(Socket(fd, port, fd_status::accepted));

    if (fd > _max_ssocket) {
      FD_SET(fd, &EventManager::_read_set);
      FD_SET(fd, &EventManager::_write_set);
    }
  } else {
    std::cerr << "Error: cannot add fd < 0" << std::endl;
  }
}

/*
 *  accept_request loops on all the active requests and tries to accept an
 *  incoming request if and only if :
 *      - there is something to accept (select event)
 *      - the socket is readable
 *      - the socket is a listening socket
 *
 *  upon successful and complete acceptance of the request, the corresponding
 *  socket will be set as 'accepted' and added to the Socket vector to be later
 *  used by accept_request() and send_response()
 */

void EventManager::accept_request(int fd) {
  (void)fd;
  for (unsigned long i = 0; i < EventManager::_sockets.size(); i++) {
    if (FD_ISSET(EventManager::_sockets[i].get_fd(),
                 &EventManager::_read_set) &&
        EventManager::_sockets[i].get_status() == fd_status::listener) {
      int tmp_fd;
      struct sockaddr_in client_addr;
      socklen_t addr_len = sizeof(client_addr);

      tmp_fd = accept(EventManager::_sockets[i].get_fd(),
                      (struct sockaddr*)&client_addr, &addr_len);
      if (tmp_fd < 0)
        perror("Accept");
      else {
        std::cout << EventManager::get_total_requests() << std::endl;
        add(tmp_fd, EventManager::_sockets[i].get_port());
        EventManager::_total_requests++;
      }
    }
  }
}

/*
 *  recv_request loops on all the active requests and tries to read and store
 *  the data if and only if :
 *      - there is something to read (select event)
 *      - the socket is readable
 *      - the socket has been previously accepted by a listening socket
 *      - the socket is not a listening socket
 *
 *  upon successful and complete receiving of the response, the corresponding
 *  socket will be set as 'read' to be later used by send_response()
 */

void EventManager::recv_request(int index) {
  (void)index;
  for (unsigned long i = 0; i < EventManager::_sockets.size(); i++) {
    if (FD_ISSET(EventManager::_sockets[i].get_fd(), &_read_set) &&
        EventManager::_sockets[i].get_status() == fd_status::accepted) {
      char buffer[4096];
      int ret;

      ret =
          recv(EventManager::_sockets[i].get_fd(), buffer, 4096, MSG_DONTWAIT);
      if (ret > 0) {
        EventManager::_sockets[i].manage_raw_request(buffer, ret);
      }
    }
  }
}

/*
 *  send_reponse loops on all the active requests, gets the reponses if it is
 *  ready and sends it (fully or partially) if and only if :
 *      - the response is ready
 *      - the socket is writable (select event)
 *      - the socket has been fully read
 *      - the socket is not a listening socket
 *
 *  upon successful and complete sending of the response, the corresponding
 *  socket will be closed and set as 'closed' to be later delete by resize()
 */

void EventManager::send_response(int index) {
  (void)index;
  for (unsigned long i = 0; i < EventManager::_sockets.size(); i++) {
    if (FD_ISSET(EventManager::_sockets[i].get_fd(), &_write_set) &&
        EventManager::_sockets[i].get_status() == fd_status::read) {
      // std::cout << "sending a chunk" << std::endl;
      usleep(1000);
      if (EventManager::_sockets[i].manage_response() ==
          RESPONSE_SENT_ENTIRELY) {
        std::cout << "Every chunk are sent" << std::endl;
        close(EventManager::_sockets[i].get_fd());
        EventManager::_sockets[i].set_status(fd_status::closed);
      }
      // TO DO : check with Brian the other cases a fd needs to be closed
    }
  }
}

/*
 *  Resize is used to delete closed sockets from the Socket vector and file
 *  descriptors from select read/write sets
 */

void EventManager::resize(void) {
  for (unsigned long i = 0; i < EventManager::_sockets.size(); i++) {
    if (EventManager::_sockets[i].get_status() == fd_status::closed) {
      FD_CLR(EventManager::_sockets[i].get_fd(), &EventManager::_read_set);
      FD_CLR(EventManager::_sockets[i].get_fd(), &EventManager::_write_set);
      EventManager::_sockets.erase(EventManager::_sockets.begin() + i);
    }
  }
}
}  // namespace network
