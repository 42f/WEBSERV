#include "Thread.hpp"

namespace network {

/*
 * Creates a Thread instance, does nothing else
 */
Thread::Thread(void) : _status(thread_status::available), _number(0) {
    pthread_mutex_init(&_lock, NULL);
    pthread_cond_init(&_cond, NULL);
}

/*
 * Destructor
 */
Thread::~Thread() {}

/*
 * Init(fn, args) : creates a thread
 */
void Thread::init(void *fn(void *), void *args) {
    if (pthread_create(&_id, NULL, fn, args) != 0) {
        std::cerr << "Error: creating thread" << std::endl;
        this->~Thread();
    }
}

/*
 * join() : joins a thread if possible, outputs error otherwise
 */
void Thread::join() {
    if (is_joinable()) {
        pthread_join(_id, NULL);
        _joinable = false;
    } else
        std::cerr << "Error: thread not joinable" << std::endl;
}

/*
 * detach() : detach a thread if possible, outputs error otherwise
 */
void Thread::detach() {
    if (pthread_detach(_id) != 0) {
        _joinable = false;
        // std::cout << strerror(errno) << std::endl;
        std::cerr << "Non c'est bon en fait" << std::endl;
    } else {
        std::cerr << "Error: thread not here I guess joinable" << std::endl;
    }
}

void Thread::thread_sleep(void) {
    pthread_mutex_lock(&_lock);
    pthread_cond_wait(&_cond, &_lock);
}

void Thread::end_work(void) {
    _status = thread_status::available;
    pthread_mutex_unlock(&_lock);
}

int Thread::wake(void) {
    pthread_mutex_lock(&_lock);
    pthread_cond_signal(&_cond);
    pthread_mutex_unlock(&_lock);
    return (0);
}

/*
 *  Returns true if the current instance is joinable. false otherwise
 */
bool Thread::is_joinable() const { return (_joinable); }
// void Thread::set_fd(int fd) { _fd = fd; }
void Thread::set_number(int value) { _number = value; }
int Thread::get_number(void) { return _number; }
void Thread::set_status(thread_status::status status) { _status = status; }

// int Thread::get_fd(void) const { return _fd; }

void Thread::set_key(int value) {_key = value;}
int Thread::get_key(void) {return _key;}

pthread_t Thread::get_id(void) const { return _id; }
thread_status::status Thread::get_status(void) const { return _status; }

Thread &Thread::operator=(Thread const &rhs) {
    if (this != &rhs) {
        _cond = rhs._cond;
        _lock = rhs._lock;
        _id = rhs._id;
        _joinable = rhs._joinable;
        // _fd = rhs._fd;
    }
    return *this;
}
}  // namespace network