//
// Created by alena on 16/06/2021.
//

#ifndef WEBSERV_CONSTANTS_HPP
#define WEBSERV_CONSTANTS_HPP

/*
 * info level: default. No extended error tracing
 */

#define LOG_LEVEL_INFO	0
#define DEBUG_MODE 		1

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

#ifndef TYPES_MIME_CONF_PATH
# define TYPES_MIME_CONF_PATH "./assets/conf_types/types_mime.conf"
#endif

#ifndef DEFAULT_CONTENT_TYPE
# define DEFAULT_CONTENT_TYPE "application/octet-stream"
#endif

#ifndef DEFAULT_SEND_SIZE
# define DEFAULT_SEND_SIZE 1024
#endif

#ifndef DEFAULT_MAX_OPEN_FD
# define DEFAULT_MAX_OPEN_FD 512
#endif

#define WEBSERV_NAME			"Webserv Team ABC"

#define RESPONSE_AVAILABLE		0
#define RESPONSE_SENT_ENTIRELY	-1
#define RESPONSE_NO_FD			-1

#define PARAM_REDIR_REQ_SCHEME	"$scheme"
#define PARAM_REDIR_REQ_URI		"$request_uri"
#define PARAM_REDIR_REQ_QUERY	"$request_query"

#define CGI_TIMEOUT				2 // TODO check if correct ?

#define UNSET					-1

#endif  // WEBSERV_CONSTANTS_HPP
