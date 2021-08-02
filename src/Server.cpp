//
// Created by alena on 03/06/2021.
//

#include "Server.hpp"
#include "export.hpp"
#include <vector>
#include <cerrno>
namespace tcp {

	Server::Server() {
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in addr;

		//Creation du socket
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(3000);
		addr.sin_addr.s_addr = inet_addr("0.0.0.0");
		bool value = true;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
		if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) || listen(sock, 256)) {
			throw Server::ServerException(strerror(errno));
		};
		fcntl(sock, F_SETFL, O_NONBLOCK);
		_socket = sock;
	}

	void Server::incoming() {
		struct sockaddr_in client;
		socklen_t len = sizeof(client);
		int sock;

		// Attente d'un client
		if ((sock = accept(_socket, (struct sockaddr *) &client, &len)) != -1) {
			fcntl(sock, F_SETFL, O_NONBLOCK);
			std::cout << "new client !" << std::endl;
			char ip[20];
			inet_ntop(AF_INET, &client, ip, len);
			std::cout << "new client connected from " << ip << std::endl;
			_clients.insert(std::make_pair(sock, Client()));
		}
	}

	void Server::poll() {
		fd_set readSet;
		struct timeval timeout;
		int max_fd = 0;

		bzero(&timeout, sizeof(timeout));
		FD_ZERO(&readSet);
		for (_clientsIterator it = _clients.begin(); it != _clients.end(); it++) {
			FD_SET(it->first, &readSet);
			max_fd = std::max(max_fd, it->first);
		}
		if (select(max_fd + 1, &readSet, NULL, NULL, &timeout) == -1)
			throw Server::ServerException(strerror(errno));

		std::vector<int> disconnected;

		for (_clientsIterator it = _clients.begin(); it != _clients.end(); it++) {
			if (FD_ISSET(it->first, &readSet)) {
				char buff[1024];
				int n = recv(it->first, buff, 1024, MSG_DONTWAIT);
				if (n <= 0) {
					std::cout << "done" << std::endl;
					disconnected.push_back(it->first);
				} else {
					it->second._req.update(buff, n);
				}
			}
		}
		for (std::vector<int>::iterator it = disconnected.begin(); it != disconnected.end(); it++) {
			close(*it);
			_clients.erase(*it);
		}
	}

	void Server::start() {
		bool running = true;
		while (running) {
			incoming();
			poll();
		}
	}

	Server::~Server() {
		std::cout << "bye bye" << std::endl;
		close(_socket);
	}

	Server::ServerException::ServerException() throw(): _error("unknown error") {}

	Server::ServerException::ServerException(std::string error) throw(): _error(error) {}

	Server::ServerException::~ServerException() throw() {}

	const char *Server::ServerException::what() const throw() {
		return this->_error.c_str();
	}
}
/*
 *
 * POST / HTTP/1.1
 * Content-Type: text/html
 * Host: localhost:3000
 * Content-Length: 12
 *
 * Hello worldGET / HTTP/1.1
 *
 */