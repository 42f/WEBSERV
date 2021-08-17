#pragma once

# include <time.h>
# include <iostream>
# include <string>
# include <string.h>

class Timer	{

    public:

        Timer( void ) {};
        ~Timer( void ) {};

        void            start(void) { _start = clock(); };
        unsigned int    getTimeElapsed( void ) {
            return (clock() - _start);
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

    private:

        time_t  _start;

        Timer( Timer const & src );
        Timer &		operator=( Timer const & rhs );
};

