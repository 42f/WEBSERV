#pragma once

# include <time.h>
# include <iostream>
# include <string>

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
            struct tm * curtime = localtime ( &_tm );
            std::string date = asctime(curtime);

            return date.substr(0, date.find('\n'));
        }

    private:

        time_t  _start;

        Timer( Timer const & src );
        Timer &		operator=( Timer const & rhs );
};

