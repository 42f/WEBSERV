#include <unistd.h>

#include "src/Network/ThreadPool.hpp"

void *routine(void *args) {
    network::Thread *self = (network::Thread *)args;

    while (1) {
        std::cout << "before sleep" << std::endl;
        self->begin_wait();
        std::cout << self->get_id() << std::endl;
        self->end_wait();
        std::cout << "after sleep" << std::endl;
    }
    return (NULL);
}

int main(int argc, char **argv) {
    network::ThreadPool pool(4);
    int i;

    pool.init(routine);
    for (;;) {
        std::cin >> i;
        pool[i].wake();
    }
    return (0);
}