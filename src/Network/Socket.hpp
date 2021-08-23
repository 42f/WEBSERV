#ifndef NETWORK_SOCKET_HPP
#define NETWORK_SOCKET_HPP

#include "HTTP/RequestHandler.hpp"
#include "HTTP/Response/ResponseHandler.hpp"
#include <netinet/in.h>

namespace fd_status {
enum status { error, listener, accepted, read, written, closed };
}

namespace network {

/*
 * Sockets are used to store informations and data about a client's request
 */

class Socket {
   public:
    typedef Result<Request, status::StatusCode> result_type;

    Socket(int fd, int port, char *client_ip, fd_status::status status = fd_status::error);
    Socket(Socket const & src);
    Socket(void);
    ~Socket();

    void set_flags(int flags);
    void set_has_events(bool value);
    void set_status(fd_status::status status);

    int get_fd(void) const;
    int get_port(void) const;
    int get_flags(void) const;
    bool has_events(void) const;
    fd_status::status get_status(void) const;
    Response const & get_response(void) const;
    bool response_is_ready(void);
    char *get_client_ip(void) const;

    int manage_response();
    void manage_raw_request(char *buffer, int size);

    Socket &operator=(Socket const &rhs);

   private:
    int _fd;
    int _port;
    int _flags;
    bool _has_events;
    bool _is_processed;
    std::string _buffer;
    fd_status::status _status;
    RequestHandler _request_handler;
    RequestHandler::result_type _res;
    ResponseHandler _response_handler;
    Response _response;
    char *_client_ip;
};

}  // namespace network

#endif
