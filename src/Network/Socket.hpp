#ifndef NETWORK_SOCKET_HPP
#define NETWORK_SOCKET_HPP

#include "HTTP/RequestHandler.hpp"

namespace fd_status {
enum status { error, listener, accepted, read, written, closed };
}

namespace network {
class Socket {
   public:
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
	std::string request;

   private:
    Socket(void);
    int _fd;
    bool _has_events;
    int _flags;
    fd_status::status _status;
    RequestHandler _handler;
};

}  // namespace network

#endif