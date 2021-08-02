
#include "Config/ConfigParser.hpp"
#include "HTTP/RequestHandler.hpp"
#include "HTTP/Response/Response.hpp"
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
#include <pthread.h>

#define BACKLOG		100 //nb of connection queued when listen is called
#define MAXLINE		1024
#define SERVER_PORT 18000
#define SA struct sockaddr

void	fake_workload( int req_id )	{
	int	sleep_time = rand() & 0xFFFFF;
	if (req_id % 2 == 0)
		sleep_time *= 2;
	std::cout << "Sleep for... " << sleep_time << " us." << std::endl;
	usleep(sleep_time);
}

struct Connection {

	std::string 	client_addr;
	int 			req_counter;
	int 			connfd;
	ParserResult<std::vector<config::Server> > cfgs;
};


void*	conn_reader(void *connection_data ) {

	Connection	*c = (Connection*)connection_data;

	char	recvline[MAXLINE+1];
	int		n;
	memset(recvline, 0, MAXLINE);
	n = read(c->connfd, recvline, MAXLINE-1);
	if (n < 0)
		exit (1);

	RequestHandler	handler;

	std::string		request(recvline);
	std::cout << request << std::endl;

	std::cout << BLUE << std::endl;
	RequestHandler::result_type res = handler.update(request.c_str(), request.length());
	std::cout << NC << std::endl;
	if (res.is_ok()) {
		Request req = res.unwrap();

		// TODO we're in the client. Check the associated config::Server for a
		// with config::Server::is_match
		// then call config::Server::handle(req);

		// default response, will be replace by response manager
		fake_workload(c->req_counter);

		Response resp(res.unwrap().version, status::Ok);


		resp.set_header(Header(slice("coucou"), slice("42")));

		std::cout << resp << std::endl;


		std::ostringstream output;
		output << resp;
		output << "\r\n";
		output << c->req_counter << " - Hello [" << c->client_addr << "]";
		output << "\r\n\r\n";
		write(c->connfd, output.str().c_str(), output.str().length());
	}
	close(c->connfd);
	free(connection_data);
	return NULL;
}

void	simple_listener( ParserResult<std::vector<config::Server> >&cfgs ) {

	int					req_counter = 0;
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	struct sockaddr_in	servaddr_client;

	// getting the fd of the socket
	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit(1);

	int iSetOption = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

	// setup the server address infos before biding
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

	if ( bind(listenfd, (SA*)&servaddr, sizeof(servaddr)) < 0 )
		exit(2);
	if ( listen(listenfd, BACKLOG) < 0 )
		exit(3);

	// infinit loop to listen to the port and respond
	while (true)	{

		printf("[#%d] Waiting for connection on port %d...\n", req_counter++, SERVER_PORT);
		fflush(stdout);
		bzero(&servaddr, sizeof(servaddr_client));
		int len = sizeof(servaddr_client);
		// will block until something comes to the port
		connfd = accept(listenfd, (struct sockaddr *)&servaddr_client, (socklen_t *)&len);

		char client_addr[32];
		bzero(&client_addr, 32);
		inet_ntop(AF_INET, &(servaddr_client.sin_addr), client_addr, 32);
		printf("client ip: %s\n",  client_addr);


		Connection	*c = (Connection*)malloc(sizeof(Connection));
		bzero(c, sizeof(Connection));
		c->client_addr.insert(0, client_addr);
		c->connfd = connfd;
		c->req_counter = req_counter;
		c->cfgs = cfgs;

		//threaded version :
		// pthread_t	t;
		// pthread_create(&t, NULL, &conn_reader, c);

		//non threaded version :
		conn_reader(c);

	} //-- end while
}

int main(int ac, char **av)
{
	Logger::getInstance("./logg", Logger::toConsole);
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
#if LOG_LEVEL == LOG_LEVEL_TRACE
	cfgs.unwrap_err().trace(cfg);
#endif
	} else {
		simple_listener(cfgs);
	}

	return 0;
}
