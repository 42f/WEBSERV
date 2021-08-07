#ifndef NETWORK_SOCKET_HPP
#define NETWORK_SOCKET_HPP

namespace fd_status {
enum status { error, is_listener, accepted, read, written, closed };
}

namespace network {
class Socket {
   public:
    Socket(int fd, fd_status::status status = fd_status::error);
    ~Socket();
    int get_fd() const;
    fd_status::status get_status() const;
    void set_status(fd_status::status status);

    bool is_ready(void) const;

   private:
    Socket(void);
    int _fd;
    fd_status::status _status;
};

}  // namespace network

#endif