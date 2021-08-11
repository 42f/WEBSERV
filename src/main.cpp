
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <utility>

#include "Config/ConfigParser.hpp"
#include "Config/Server.hpp"
#include "HTTP/RequestHandler.hpp"
#include "HTTP/Response/Response.hpp"
#include "HTTP/Response/ResponseHandler.hpp"
#include "Network/Core.hpp"
#include "Network/ServerPool.hpp"
#include "utils/Logger.hpp"

#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/uio.h>
#include <unistd.h>
#include <utility>
#include <pthread.h>

#define BACKLOG		100 //nb of connection queued when listen is called
#define MAXLINE		1024
#define SERVER_PORT 18000
// #define SERVER_PORT 8080
#define SA struct sockaddr

void exit_server(int sig) {
    (void)sig;
    std::cout << "\rGot signal, Bye..." << std::endl;
    exit(0);
}

int main(int ac, char **av) {
    // signal(SIGINT, &exit_server);
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
    Logger::getInstance("./logg", Logger::toConsole);

    std::set<int> ports = network::ServerPool::getPorts();
    std::vector<network::ServerSocket> sockets(ports.begin(), ports.end());

    network::Core core(sockets);
    core.run_servers();

    return 0;
}

