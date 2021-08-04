
#include "Config/ConfigParser.hpp"
#include "HTTP/RequestHandler.hpp"
#include "Server.hpp"



int main(int ac, char **av)
{
	std::string 	path;
	switch (ac) {
		case 1:
			path = "webserv.config";
			break;
		case 2:
			path = av[1];
			break;
		default:
			std::cerr << "./webserv [ConfigServerv]" << std::endl;
			return -1;
	}
	Logger::getInstance("./webserv.log");

	std::vector<config::Server>		servers = config::parse(path);

	RequestHandler	handler;

	std::string		request = "GET / HTTP/1.1\r\nHost:		localhost	 	\r\nContent-Length: 12		\r\n\r\nHello world!GET ";
	Result<Request, status::StatusCode> res = handler.update(request.c_str(), request.length());
	if (res.is_ok()) {
		Request req = res.unwrap();
		// TODO we're in the client. Check the associated config::Server for a
		// with config::Server::is_match
		// then call config::Server::handle(req);
	}
	if (res.is_err())
	{
		if (res.unwrap_err() != status::None) {
			std::cerr << res.unwrap_err() << " " << status::message(res.unwrap_err()) << std::endl;
		}
	}

	//tcp::Server	server;

	//server.start();
	return 0;
}
