//
// Created by alena on 03/06/2021.
//

#ifndef WEBSERV_TCP_SERVER_HPP
#define WEBSERV_TCP_SERVER_HPP

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <iostream>
#include <exception>
#include <string>
#include <cstring>
#include <unistd.h>

#include <map>
#include "Constants.hpp"
#include <vector>
#include "HTTP/RequestHandler.hpp"

struct Response {
	int		status;
	char 	buf[];
};

class Client {
private:
	int					_socket;

public:
	RequestHandler		_req;

	Client() {}

	void 				send(const Response& res)
	{
		(void)res;
//		send(_socket, ...)
	}
};

namespace tcp {

	class Server {
	private:
		int											_socket;
		std::map<int, Client>						_clients;
		typedef std::map<int, Client>::iterator		_clientsIterator;

	public:
		Server();
		~Server();

		void start();
		void incoming();
		void poll();

		class ServerException: public std::exception {
		private:
			std::string		_error;

		public:

			ServerException() throw();
			virtual ~ServerException() throw();

			virtual const char* what() const throw();

			ServerException(std::string error) throw();

		};
	};

}

#endif //WEBSERV_SERVER_HPP
