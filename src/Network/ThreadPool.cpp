#include "ThreadPool.hpp"

namespace network {
ThreadPool::ThreadPool(void) {}
ThreadPool::~ThreadPool() {}

void ThreadPool::create(int size) {
    for (int i = 0; i < size; i++) {
        Thread p;
        _pool.push_back(p);
    }
}
void ThreadPool::init(void *fn(void *args)) {
    for (int i = 0; i < static_cast<int>(_pool.size()); i++) {
        _pool[i].init(fn, &_pool[i]);
    }
}

int ThreadPool::size(void) const  { return _pool.size(); }

Thread &ThreadPool::operator[](int index) { return (_pool[index]); }
ThreadPool &ThreadPool::operator=(ThreadPool const &src) {
    if (this != &src) {
        _pool = src._pool;
    }
    return *this;
}

}  // namespace network