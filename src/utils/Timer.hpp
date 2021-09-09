#pragma once

# include <time.h>
# include <iostream>
# include <string>
# include <string.h>

class Timer	{

    public:

        Timer( void ) : _start(time(NULL)) {};
        ~Timer( void ) {};
        Timer( Timer const & src ) {
           *this = src;
        };

        void            start(void) { _start = time(NULL); };
        unsigned int    getTimeElapsed( void ) const {
            return (time(NULL) - _start);
        };

        static std::string  getTimeNow( void ) {
            time_t _tm = time(NULL);
            return getTimeStr(localtime(&_tm));
        }

        static std::string  getTimeStr( struct tm * time ) {
            char buff[30];
            bzero(buff, 30);
            strftime(buff, 29, "%a, %d %b %Y %H:%M:%S %Z", time);
            return buff;
        }
        Timer &		operator=( Timer const & rhs ) {
            if (this != &rhs) {
                _start = rhs._start;
            }
            return *this;
        };

    private:

        time_t  _start;
};

