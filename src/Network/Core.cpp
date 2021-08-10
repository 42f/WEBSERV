#include "Core.hpp"

namespace network {

#if POLL_FN == KQUEUE
void *ok(void *args) {
    Thread *self = static_cast<Thread *>(args);
    for (;;) {
        self->thread_sleep();
        EventsManager::send_response(self->get_key());
        self->end_work();
    }

    return (NULL);
}

Core::Core(std::vector<network::ServerSocket> s, int tpool_size)
    : _nb_events(0) {
    EventsManager::init(s);
    _tpool.create(tpool_size);
    _tpool.init(ok);
}
Core::~Core() {}

void Core::update_events(void) {
    EventsManager::do_kevent();
    if (EventsManager::get_nb_events() < 0) {
        std::cerr << "Error Kevent : system failure" << std::endl;
    } else {
        _nb_events = EventsManager::get_nb_events();
    }
}

void Core::check_sockets(void) {
    if (EventsManager::get_nb_events() > 0) {
        for (int i = 0; i < EventsManager::get_nb_events(); i++) {
            int fd = EventsManager::get_event_struct(i).ident;
            if (EventsManager::is_acceptable(fd)) {
                EventsManager::accept_request(fd);
            }
        }
    }
}

void Core::check_requests(void) {
    if (EventsManager::get_nb_events() > 0) {
        for (int i = 0; i < EventsManager::get_nb_events(); i++) {
            int fd = EventsManager::get_event_struct(i).ident;
            if (EventsManager::is_readable(fd)) {
                EventsManager::recv_request(fd);
            }
        }
    }
}

void Core::check_responses(void) {
    for (int i = 0; i < EventsManager::get_nb_events(); i++) {
        int fd = EventsManager::get_event_struct(i).ident;
        if (EventsManager::is_writable(fd)) {
            EventsManager::send_response(fd);

            // for (int i = 0; i < _tpool.size(); i++) {
            //     if (_tpool[i].get_status() == thread_status::available) {
            //         _tpool[i].set_status(thread_status::busy);
            //         _tpool[i].set_number(_tpool[i].get_number() + 1);
            //         _tpool[i].set_key(fd);
            //         _tpool[i].wake();
            //         break;
            //     }
            // }
        }
    }
    EventsManager::resize();
}

void Core::run_servers(void) {
    for (;;) {
        update_events();
        check_sockets();
        check_requests();
        check_responses();
    }
}
#elif POLL_FN == SELECT

void *ok(void *args) {
    Thread *self = static_cast<Thread *>(args);
    for (;;) {
        self->thread_sleep();
        EventsManager::send_response(self->get_key());
        self->end_work();
    }

    return (NULL);
}

Core::Core(std::vector<network::ServerSocket> s, int tpool_size)
    : _nb_events(0) {
    EventsManager::init(s);
    _tpool.create(tpool_size);
    _tpool.init(ok);
}
Core::~Core() {}

void Core::update_events(void) {
    EventsManager::do_kevent();
    if (EventsManager::get_nb_events() < 0) {
        std::cerr << "Error Kevent : system failure" << std::endl;
    } else {
        _nb_events = EventsManager::get_nb_events();
    }
}

void Core::check_sockets(void) {
    if (EventsManager::get_nb_events() > 0) {
#if POLL_FN == KQUEUE
        for (int i = 0; i < EventsManager::get_nb_events(); i++) {
            int fd = EventsManager::get_event_struct(i).ident;
            if (EventsManager::is_acceptable(fd)) {
                EventsManager::accept_request(fd);
            }
        }
#elif POLL_FN == SELECT
        EventsManager::accept_request(0);
#endif
    }
}

void Core::check_requests(void) {
#if POLL_FN == KQUEUE

    if (EventsManager::get_nb_events() > 0) {
        for (int i = 0; i < EventsManager::get_nb_events(); i++) {
            int fd = EventsManager::get_event_struct(i).ident;
            if (EventsManager::is_readable(fd)) {
                EventsManager::recv_request(fd);
            }
        }
    }
#elif POLL_FN == SELECT
    EventsManager::recv_request(0);
#endif
}

void Core::check_responses(void) {
#if POLL_FN == KQUEUE
    for (int i = 0; i < EventsManager::get_nb_events(); i++) {
        int fd = EventsManager::get_event_struct(i).ident;
        if (EventsManager::is_writable(fd)) {
            EventsManager::send_response(fd);
        }
    }
#elif POLL_FN == SELECT
    EventsManager::send_response(0);
#endif
    EventsManager::resize();
}

void Core::run_servers(void) {
    for (;;) {
        update_events();
        check_sockets();
        check_requests();
        check_responses();
    }
}

#endif

}  // namespace network
