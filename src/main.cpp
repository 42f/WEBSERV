
#include "Config/ConfigParser.hpp"
#include "HTTP/RequestHandler.hpp"
#include "Server.hpp"
#include <fstream>
#include <sstream>

struct Params {
	std::string 	config_file;
	std::string 	request;

	Params(): config_file("webserv.config"), request("request.test") { }
};

Result<Params>	parse(int ac, char **av)
{
	std::string 	path;
	Params			params;

	for (int i = 0; i < ac; i++) {
		std::string arg = std::string(av[i]);
		if (arg == "-c")
		{
			if (i == ac - 1)
				return Result<Params>::err(DefaultError("missing argument"));
			params.config_file = std::string(av[++i]);
		}
		if (arg == "-r")
		{
			if (i == ac - 1)
				return Result<Params>::err(DefaultError("missing argument"));
			params.request = std::string(av[++i]);
		}
	}
	return Result<Params>::ok(params);
}

int main(int ac, char **av)
{
	Result<Params> r = parse(ac - 1, av + 1);
	if (r.is_err()) {
		std::cerr << r.unwrap_err().what() << std::endl;
		return -1;
	}
	Params params = r.unwrap();

	Logger::getInstance("./webserv.log");

	std::vector<config::Server>		servers = config::parse(params.config_file);

	RequestHandler	handler;

	std::ifstream t(params.request.c_str());
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string		request = buffer.str();
	Result<Request, status::StatusCode> res = handler.update(request.c_str(), request.length());
	if (res.is_ok()) {
		Request req = res.unwrap();
		std::cout << req << std::endl;
		// TODO we're in the client. Check the associated config::Server for a
		// with config::Server::is_match
		// then call config::Server::handle(req);
	}
	if (res.is_err())
	{
		if (res.unwrap_err() != status::Incomplete) {
			std::cerr << res.unwrap_err() << " " << status::message(res.unwrap_err()) << std::endl;
		}
		else
			std::cerr << "incomplete, waiting for more data" << std::endl;
	}

	tcp::Server	server;

	server.start();
	return 0;
}

