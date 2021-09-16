#include "EventManager.hpp"

#define FULL_SKT_RD(fd, status) \
  (FD_ISSET(fd, &EventManager::_read_set) && HAS_SKT_READABLE(status))
#define FULL_SKT_WR(fd, status) \
  (FD_ISSET(fd, &EventManager::_write_set) && HAS_SKT_WRITABLE(status))
#define FULL_OFD_RD(fd, status) \
  (FD_ISSET(fd, &EventManager::_read_set) && HAS_OFD_USABLE(status))
#define FULL_OFD_WR(fd, status) \
  (FD_ISSET(fd, &EventManager::_write_set) && HAS_OFD_USABLE(status))

namespace network {
int EventManager::_timeout = 0;
int EventManager::_nb_events = 0;
int EventManager::_total_requests = 0;
fd_set EventManager::_read_set;
fd_set EventManager::_write_set;
int EventManager::_max_fd = 0;
std::list<Socket> EventManager::_sockets;

/***************************************************
    Getters
***************************************************/

/*
 *	init() initializes the vector of active sockets and the first
 *select read & write sets with the listening sockets. Also, init()
 *sets the maximum listening socket number
 */

void EventManager::init(std::vector<network::ServerSocket> s) {
  int i = 0;
  for (std::vector<network::ServerSocket>::iterator it = s.begin();
       it != s.end(); it++) {
    FD_SET(it->get_id(), &EventManager::_read_set);
    EventManager::_sockets.push_back(
        Socket(it->get_id(), it->get_port(), NULL, fd_status::listenable));
    if (it->get_id() > _max_fd) _max_fd = it->get_id();
    i++;
  }
}

EventManager::~EventManager() {}

/***************************************************
    Getters
***************************************************/

std::list<Socket> const &EventManager::get_sockets(void) { return _sockets; }

unsigned long EventManager::get_size(void) {
  return EventManager::_sockets.size();
}

int EventManager::get_nb_events(void) { return EventManager::_nb_events; }

int EventManager::get_total_requests(void) { return _total_requests; }

/***************************************************
    Functions
***************************************************/

/*
 *  do_select() clears select write & read sets, populate them with
 * the file descriptors from the active sockets vector and calls the
 * select() function on all active sockets (listeners or not)
 */

void EventManager::do_select(void) {
  FD_ZERO(&EventManager::_read_set);
  FD_ZERO(&EventManager::_write_set);
  _max_fd = 0;

  std::list<Socket>::iterator itr;
  for (itr = EventManager::_sockets.begin();
       itr != EventManager::_sockets.end(); ++itr) {
    int skfd =itr->get_skt_fd();
    int ofd =itr->get_o_fd();
    int ufd =itr->get_u_fd();
    int status = itr->get_status();

    if (IS_LISTENABLE(status)) {
      _max_fd = (skfd > _max_fd) ? skfd : _max_fd;
      FD_SET(skfd, &EventManager::_read_set);
    } else if (!HAS_ERROR(status)) {
      _max_fd = (skfd > _max_fd) ? skfd : _max_fd;
      FD_SET(skfd, &EventManager::_read_set);
      FD_SET(skfd, &EventManager::_write_set);

      if (ufd != UNSET) {
        _max_fd = (ufd > _max_fd) ? ufd : _max_fd;
        FD_SET(ufd, &EventManager::_write_set);
      }
      if (ofd != UNSET) {
        _max_fd = (ofd > _max_fd) ? ofd : _max_fd;
        FD_SET(ofd, &EventManager::_read_set);
      }
    }
  }
  EventManager::_nb_events =
      select(EventManager::_max_fd + 1, &EventManager::_read_set,
             &EventManager::_write_set, NULL, NULL);
  if (EventManager::_nb_events < 0) {
    perror("select");
    std::cerr << EventManager::_max_fd + 1 << std::endl;
    exit(1);
  }
}

/*
 *  add creates a socket giving it an fd and a port, pushes it in the
 * Active Socket vector and adds the fd into select read & write sets
 *
 *  add() only adds non-listening soccket
 */

void EventManager::add(int fd, int port, struct sockaddr_in client_addr) {
  if (fd > 0) {
    if (fd > EventManager::_max_fd) EventManager::_max_fd = fd;
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    EventManager::_sockets.push_back(
        Socket(fd, port, client_ip, fd_status::skt_readable));
  } else {
    std::cerr << "Error: cannot add fd < 0" << std::endl;
  }
}

/*
 *  accept_request loops on all the active requests and tries to
 * accept an incoming request if and only if :
 *      - there is something to accept (select event)
 *      - the socket is readable
 *      - the socket is a listening socket
 *
 *  upon successful and complete acceptance of the request, the
 * corresponding socket will be set as 'accepted' and added to the
 * Socket vector to be later used by accept_request() and
 * send_response()
 */

void EventManager::accept_request(void) {
  std::list<Socket>::iterator itr;
  for (itr = EventManager::_sockets.begin();
       itr != EventManager::_sockets.end(); ++itr) {
    if (FD_ISSET(itr->get_skt_fd(), &EventManager::_read_set) &&
        IS_LISTENABLE(itr->get_status())) {
      int tmp_fd;
      struct sockaddr_in client_addr;
      socklen_t addr_len = sizeof(client_addr);

      tmp_fd =
          accept(itr->get_skt_fd(), (struct sockaddr *)&client_addr, &addr_len);
      if (tmp_fd < 0)
        perror("Accept");
      else {
        add(tmp_fd, itr->get_port(), client_addr);
        EventManager::_total_requests++;
      }
    }
  }
}

/*
 *  recv_request loops on all the active requests and tries to read
 * and store the data if and only if :
 *      - there is something to read (select event)
 *      - the socket is readable
 *      - the socket has been previously accepted by a listening
 * socket
 *      - the socket is not a listening socket
 *
 *  upon successful and complete receiving of the response, the
 * corresponding socket will be set as 'read' to be later used by
 * send_response()
 */

void EventManager::recv_request(void) {
  std::list<Socket>::iterator itr;
  for (itr = EventManager::_sockets.begin();
       itr != EventManager::_sockets.end(); ++itr) {
    if (FULL_SKT_RD(itr->get_skt_fd(), itr->get_status())) {
      char buffer[4096];
      int ret;

      ret = recv(itr->get_skt_fd(), buffer, 4096, MSG_DONTWAIT);
      if (ret > 0) {
        itr->manage_raw_request(buffer, ret);
      }
    }
  }
}

/*
 *  send_reponse loops on all the active requests, gets the reponses
 * if it is ready and sends it (fully or partially) if and only if :
 *      - the response is ready
 *      - the socket is writable (select event)
 *      - the socket has been fully read
 *      - the socket is not a listening socket
 *
 *  upon successful and complete sending of the response, the
 * corresponding socket will be closed and set as 'closed' to be later
 * delete by resize()
 */

void EventManager::send_response(void) {
  std::list<Socket>::iterator itr;
  for (itr = EventManager::_sockets.begin();
       itr != EventManager::_sockets.end(); ++itr) {
    int st = itr->get_status();
    if (FULL_SKT_WR(itr->get_skt_fd(), st)) {

      itr->process_request();

      int ufd = itr->get_u_fd();
      if (ufd != UNSET && FD_ISSET(ufd, &EventManager::_write_set)) {
        itr->write_body();
      }

      st = itr->get_status();
      int ofd = itr->get_o_fd();
      if (ofd == UNSET || FD_ISSET(ofd, &EventManager::_read_set)) {
        if (itr->do_send() == RESPONSE_SENT_ENTIRELY) {
          itr->unset_status(fd_status::skt_writable);
          itr->set_status(fd_status::skt_closable);
        }
      }
    }
  }
}

/*
 *  Resize is used to delete closed sockets from the Socket vector and
 * file descriptors from select read/write sets
 */

void EventManager::resize(void) {
  std::list<Socket>::iterator itr;
  for (itr = EventManager::_sockets.begin();
       itr != EventManager::_sockets.end();) {
    if (HAS_SKT_CLOSABLE(itr->get_status())) {
      close(itr->get_skt_fd());
      std::list<Socket>::iterator itr_tmp = itr;
      itr++;
      EventManager::_sockets.erase(itr_tmp);
    } else {
      itr++;
    }
  }
  static size_t max = 0;
  if (_sockets.size() > max) max = _sockets.size();
}
}  // namespace network
