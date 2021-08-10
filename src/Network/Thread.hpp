#ifndef NETWORK_THREAD_HPP
#define NETWORK_THREAD_HPP

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include <iostream>

#include "Socket.hpp"

namespace thread_status {
enum status { available, busy, error };
}

namespace network {

class Thread {
   public:
    /* Constructor and destructor */
    Thread(void);
    virtual ~Thread();

    /* Getters and Setters */
    pthread_t get_id(void) const;
    int get_fd(void) const;
    thread_status::status get_status(void) const;
    void set_status(thread_status::status status);
    /* Member functions */
    void init(void *fn(void *), void *args);
    void join(void);
    void detach(void);
    bool is_joinable() const;
    void thread_sleep(void);
    void end_work(void);
    int wake(void);
    void set_number(int value);
    int get_number(void);

    void set_socket(Socket const &socket);
    Socket &get_socket(void);

    Thread &operator=(Thread const &rhs);

   protected:
    pthread_cond_t _cond;
    pthread_mutex_t _lock;
    pthread_t _id;
    bool _joinable;
    thread_status::status _status;
    Socket _socket;
    int _number;
};

}  // namespace network

#endif