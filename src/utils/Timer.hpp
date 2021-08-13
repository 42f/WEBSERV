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
        
    private:

        time_t  _start;

        Timer( Timer const & src );
        Timer &		operator=( Timer const & rhs );
};

