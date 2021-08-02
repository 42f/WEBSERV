
#include "Config/ConfigParser.hpp"
#include "HTTP/RequestHandler.hpp"
#include "utils/Logger.hpp"
#include "Server.hpp"

#include <fstream>
#include <sstream>


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
	std::ifstream t(path.c_str());
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string		str = buffer.str();
	slice cfg(str);

	ParserResult<std::vector<config::Server> >	cfgs = ConfigParser()(cfg);
	if (cfgs.is_err()) {
		std::cerr << cfgs.unwrap_err() << std::endl;
#if LOG_LEVEL == LOG_LEVEL_TRACE	//start line error
		cfgs.unwrap_err().trace(cfg);
#endif
	} else {
		for (std::vector<config::Server>::iterator it = cfgs.unwrap().begin(); it != cfgs.unwrap().end(); it++) {
			std::cout << *it << std::endl;
		}
	}
	tcp::Server	server;

	server.start();

//	RequestHandler	handler;
//
//	std::string		request = "GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 12\r\n\r\nHello world!and everything after";
//	Result<Request, status::StatusCode> res = handler.update(request.c_str(), request.length());
//	if (res.is_ok()) {
//		Request req = res.unwrap();
//		//TODO we're in the client. Check the associated config::Server for a
//		// with config::Server::is_match
//		// then call config::Server::handle(req);
//	}
//	if (res.is_err())
//	{
//		if (res.unwrap_err() != status::None) {
//			std::cerr << res.unwrap_err() << " " << status::message(res.unwrap_err()) << std::endl;
//		}
//	}

	return 0;
}
