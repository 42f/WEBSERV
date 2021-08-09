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

    Socket(int fd, fd_status::status status = fd_status::error);
    ~Socket();
    int get_fd() const;
    int get_flags(void) const;
    void set_flags(int flags);
    bool has_events(void) const;
    void set_has_events(bool value);
    fd_status::status get_status() const;
    void set_status(fd_status::status status);
    void manage_raw_request(char *buffer, int size);

    bool is_ready(void) const;
    void set_buffer(std::string const &buffer);
    std::string &get_buffer(void);
    Response get_response();

   private:
    Socket(void);
    int _fd;
    bool _has_events;
    int _flags;
    fd_status::status _status;
    RequestHandler _request_handler;
    // ResponseHandler _response_handler;
    std::string _buffer;
    Result<Request, status::StatusCode> _res;
};

}  // namespace network

#endif