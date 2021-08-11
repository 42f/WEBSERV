#ifndef NETWORK_SOCKET_HPP
#define NETWORK_SOCKET_HPP

#include "HTTP/RequestHandler.hpp"
#include "HTTP/Response/ResponseHandler.hpp"

namespace fd_status {
enum status { error, listener, accepted, read, written, closed };
}

namespace network {
class Socket {
   public:
    typedef Result<Request, status::StatusCode> result_type;

    Socket(int fd, int port,
                   fd_status::status status = fd_status::error);
    Socket(void);
    ~Socket();

    void set_flags(int flags);
    void set_has_events(bool value);
    void set_status(fd_status::status status);

    void manage_raw_request(char *buffer, int size);

    int get_fd(void) const;
    int get_port(void) const;
    Response get_response();
    int get_flags(void) const;
    bool has_events(void) const;
    fd_status::status get_status() const;

    Socket &operator=(Socket const &rhs);

   private:
    int _fd;
    int _flags;
    fd_status::status _status;
    bool _has_events;
    RequestHandler _request_handler;
    std::string _buffer;
    RequestHandler::result_type _res;
    int _port;
};

}  // namespace network

#endif