
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

#include "Socket.hpp"        // network::Sockets, fd_status::status
#include "ServerSocket.hpp"  // network::ServerSockets
#include "Network/Core.hpp"


void exit_server(int) {
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
  std::cout << "\rClosing Webserv." << std::endl;
  exit(0);
}

int main(int ac, char const **av)
{
  signal(SIGINT, &exit_server);
  if (ac > 2) {
	  std::cerr << "./webserv [ConfigServerv]" << std::endl;
	  return -1;
  }
  else if (ac == 1) {
  	av[1] = "./webserv.config";
    std::cout << YELLOW << "\rUsing default config: ./webserv.config" << NC << std::endl;
  }
  network::ServerPool::init( av[1]);
  files::File::initContentTypes(TYPES_MIME_CONF_PATH);

  Logger::getInstance("./logg", Logger::toConsole);

  std::set<int> ports = network::ServerPool::getPorts();
  network::Core core(std::vector<network::ServerSocket>(ports.begin(), ports.end()));
  ports.clear();
  std::cout << GREEN << "\rWebserv is running." << NC << std::endl;
  core.run_servers();
  return 0;
}
