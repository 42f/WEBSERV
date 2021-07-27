//
// Created by alena on 16/06/2021.
//

#ifndef WEBSERV_CONSTANTS_HPP
#define WEBSERV_CONSTANTS_HPP

/*
 * info level: default. No extended error tracing
 */

#define LOG_LEVEL_INFO	0

/*
 * info level: extended. Debug data and trace included
 */
#define LOG_LEVEL_TRACE 1

#define LOG_LEVEL	LOG_LEVEL_TRACE

#define YELLOW	"\033[1;33m"
#define RED		"\033[0;31m"
#define BLUE	"\033[0;34m"
#define MAGENTA	"\033[0;35m"
#define GREEN	"\033[0;32m"
#define WHITE	"\033[0;37m"
#define NC		"\033[0m"

#endif //WEBSERV_CONSTANTS_HPP
