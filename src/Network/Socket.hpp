#ifndef NETWORK_SOCKET_HPP
#define NETWORK_SOCKET_HPP

#include <netinet/in.h>

#include "HTTP/Request/RequestHandler.hpp"
#include "HTTP/Response/ResponseHandler.hpp"

namespace fd_status {
enum status {
  empty = 1 << 0,
  listenable = 1 << 1,
  skt_acceptable = 1 << 2,
  skt_readable = 1 << 3,
  skt_writable = 1 << 4,
  skt_closable = 1 << 5,
  skt_error = 1 << 6,
  ofd_usable = 1 << 7,
  ofd_readable = 1 << 8,
  ofd_writable = 1 << 9,
  ofd_closable = 1 << 10,
  ofd_error = 1 << 11,
  error = 1 << 12,
  ofd_closed = 1 << 13,
  skt_closed = 1 << 14,
  closed = 1 << 15,
  ofd_no_need = 1 << 16
};
}

#define HAS_SKT_ERROR(x) (x & fd_status::skt_error)
#define HAS_OFD_ERROR(x) (x & fd_status::ofd_error)
#define HAS_ERROR(x) \
  (HAS_OFD_ERROR(x) || HAS_SKT_ERROR(x) || (x & fd_status::error))

#define HAS_OFD_USABLE(x) (x & fd_status::ofd_usable && !HAS_OFD_ERROR(x))

#define IS_LISTENABLE(x) (x & fd_status::listenable && !HAS_OFD_ERROR(x))

#define HAS_SKT_READABLE(x) (x & fd_status::skt_readable && !HAS_OFD_ERROR(x))
#define HAS_SKT_WRITABLE(x) (x & fd_status::skt_writable && !HAS_OFD_ERROR(x))

#define HAS_OFD_NO_NEED(x) (x & fd_status::ofd_no_need && !HAS_OFD_ERROR(x))

#define HAS_SKT_CLOSABLE(x) (x & fd_status::skt_closable)// || HAS_OFD_ERROR(x))
#define HAS_OFD_CLOSABLE(x) (x & fd_status::ofd_closable || HAS_OFD_ERROR(x))

namespace network {

/*
 * Sockets are used to store informations and data about a client's request
 */

class Socket {
 public:
  void print_status(void) {
    std::cout << "Socket " << _fd << " is actually : [" << _status << "] : ";
    if (_status == fd_status::empty) {
      std::cout << "empty ";
    }
    if (_status & fd_status::listenable) {
      std::cout << "listenable ";
    }
    if (_status & fd_status::skt_acceptable) {
      std::cout << "skt_acceptable ";
    }
    if (_status & fd_status::skt_readable) {
      std::cout << "skt_readable ";
    }
    if (_status & fd_status::skt_writable) {
      std::cout << "skt_writable ";
    }
    if (_status & fd_status::skt_closable) {
      std::cout << "skt_closable ";
    }
    if (_status & fd_status::skt_error) {
      std::cout << "skt_error ";
    }
    if (_status & fd_status::ofd_usable) {
      std::cout << "ofd_usable ";
    }
    if (_status & fd_status::ofd_readable) {
      std::cout << "ofd_readable ";
    }
    if (_status & fd_status::ofd_writable) {
      std::cout << "ofd_writable ";
    }
    if (_status & fd_status::ofd_closable) {
      std::cout << "ofd_closable  ";
    }
    if (_status & fd_status::ofd_error) {
      std::cout << "ofd_error  ";
    }
    if (_status & fd_status::error) {
      std::cout << "error  ";
    }
    if (_status & fd_status::ofd_closed) {
      std::cout << "ofd_closed  ";
    }
    if (_status & fd_status::skt_closed) {
      std::cout << "skt_closed  ";
    }
    if (_status & fd_status::closed) {
      std::cout << "closed ";
    }
    std::cout << std::endl;
  };

  typedef Result<Request, status::StatusCode> result_type;

  Socket(int fd, int port, char *client_ip, int status = fd_status::error);
  Socket(Socket const &src);
  ~Socket();

  void set_o_fd(int fd);
  void set_status(int status);
  void unset_status(int status);

  int get_cgi_pid(void) const;
  int get_skt_fd(void) const;
  int get_o_fd(void) const;
  int get_port(void) const;
  int get_status(void) const;
  Response const &get_response(void) const;
  bool response_is_ready(void);
  std::string get_client_ip(void) const;
  void process_request();
  int do_send();

  int manage_response();
  void manage_raw_request(char *buffer, int size);

  Socket &operator=(Socket const &rhs);

 private:
  int _fd;
  int _ofd;
  int _port;
  int _status;
  bool _is_processed;
  std::string _client_ip;

  RequestHandler _request_handler;
  RequestHandler::result_type _res;
  ResponseHandler _response_handler;
  Response _response;
};

}  // namespace network

#endif
