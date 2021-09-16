
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

#include "Config/ConfigParser.hpp"
#include "HTTP/Response/Response.hpp"
#include "HTTP/Response/ResponseHandler.hpp"
#include "Network/Core.hpp"


void exit_server(int sig) {
  (void)sig;
  // TODO close all pending connections and cgi pipes ?
  std::list<network::Socket>::const_iterator itr;
  for (itr = network::EventManager::get_sockets().begin();
       itr != network::EventManager::get_sockets().end(); itr++) {
    if (itr->get_cgi_pid() != UNSET) {
      kill(itr->get_cgi_pid(), SIGINT);
      waitpid(itr->get_cgi_pid(), NULL, WNOHANG);
    }
    close(itr->get_skt_fd());
  }
  std::cout << "\rOpen Connections: " << network::EventManager::get_size()
            << std::endl; // TODO remove debug
  std::cout << "\rGot signal, Bye..." << std::endl;
  exit(0);
}

int main(int ac, char **av) {
  signal(SIGINT, &exit_server);
  std::string path;
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

  network::ServerPool::init(path);
  files::File::initContentTypes(TYPES_MIME_CONF_PATH);

  Logger::getInstance("./logg", Logger::toConsole);

  std::set<int> ports = network::ServerPool::getPorts();
  std::vector<network::ServerSocket> sockets(ports.begin(), ports.end());

  network::Core core(sockets);
  core.run_servers();

  return 0;
}
