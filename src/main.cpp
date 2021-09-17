
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

#include "Socket.hpp"        // network::Sockets, fd_status::status
#include "ServerSocket.hpp"  // network::ServerSockets
#include "Network/Core.hpp"


void exit_server(int sig) {
  (void)sig;
  // TODO close all pending connections and cgi pipes ?
  std::list<network::Socket>::iterator itr;
  itr = network::EventManager::get_sockets().begin();
  while(itr != network::EventManager::get_sockets().end()) {
    if (itr->get_cgi_pid() != UNSET && kill(itr->get_cgi_pid(), SIGINT) == 0) {
      waitpid(itr->get_cgi_pid(), NULL, 0);
    }
    close(itr->get_skt_fd());
    std::list<network::Socket>::iterator itr_tmp = itr;
    itr++;
    network::EventManager::get_sockets().erase(itr_tmp);
  }
  std::cout << "\rOpen Connections: " << network::EventManager::get_size()
            << std::endl; // TODO remove debug
  std::cout << "\rGot signal, Bye..." << std::endl;
  exit(0);
}

int main(int ac, char **av)
{
  signal(SIGINT, &exit_server);
  std::string path;
  if (ac == 2)
  	path = av[1];
  else {
	  std::cerr << "./webserv [ConfigServerv]" << std::endl;
	  return -1;
  }

  network::ServerPool::init(path);
  files::File::initContentTypes(TYPES_MIME_CONF_PATH);

  Logger::getInstance("./logg", Logger::toConsole);

  std::set<int> ports = network::ServerPool::getPorts();
  network::Core core(std::vector<network::ServerSocket>(ports.begin(), ports.end()));
  ports.clear();
  core.run_servers();
  return 0;
}
