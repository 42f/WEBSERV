#ifndef NETWORK_THREADPOOL_HPP
#define NETWORK_THREADPOOL_HPP

#include <vector>

#include "Thread.hpp"

namespace network {
class ThreadPool {
   public:
    ThreadPool(void);
    ~ThreadPool();
    void create(int size);
    void init(void *fn(void *args));
    int size(void) const ;

    Thread &operator[](int index);
    ThreadPool &operator=(ThreadPool const &src);

   private:
    std::vector<Thread> _pool;
};
}  // namespace network

#endif