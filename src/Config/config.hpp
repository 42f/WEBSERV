//
// Created by alena on 06/07/2021.
//

#ifndef WEBSERV_CONFIG_HPP
#define WEBSERV_CONFIG_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

#include "Config/Directives/Cgi.hpp"
#include "Config/Directives/ClientMaxBodySize.hpp"
#include "Config/Directives/ErrorPage.hpp"
#include "Config/Directives/Listen.hpp"
#include "Config/Directives/ServerName.hpp"
#include "Config/Directives/Root.hpp"
#include "Config/Directives/Index.hpp"
#include "Config/Directives/Location.hpp"

#include "Config/Structure/Bloc.hpp"
#include "Config/Structure/Line.hpp"

#include "HTTP/Request.hpp"

#include <map>

namespace config
{
/*
 * Result of server block
 */
	class Server
	{
	private:
		int 			_port;
		std::string		_address;
		std::string		_name;
		std::string		_root;
		std::string		_index;
		size_t			_body_size;		// in bytes
		std::vector<LocationConfig>			_locations;
		std::map<int, std::string>			_error_pages;
		std::map<std::string, std::string>	_cgis;

		static Server		*active;

		Result<std::string>			error_page(int status_code);
		Result<std::string>			cgi_binary(std::string ext);

	public:

		Server();

		static	void	use(Server *ptr);

		/*
		 * SETTER
		 */
		static Server	*with_addr(tuple<std::string, int> addr);
		static Server	*dump(slice unused);
		static Server	*name(slice name);
		static Server	*max_size(size_t max_size);
		static Server	*root(slice root);
		static Server	*cgi(tuple<slice, slice> cgi);
		static Server	*error_codes(std::vector<std::pair<int, std::string> > errors);
		static Server	*index(slice index);
		static Server	*with_location(LocationConfig config);

		/*
		 * GETTER
		 */
		unsigned short 				get_port();
		std::string					get_addr();

		/*
		 * RESPONSE
		 */
		// TODO match server
		bool is_match(std::string host, const Request &req);

		struct Response {  };
		Result<Response, int>	handle(const Request &req);

		friend std::ostream &operator<<(std::ostream& stream, const Server& cfg);
	};
/*
 * ServerContent is a line for block server
 * listen | server_name ...
 */

//TODO cpp
	class ServerContent: public Parser<Server>
	{
	public:
		ServerContent() { }

		result_type 	operator()(const slice &input)
		{
			Server	cfg;
			Server::use(&cfg);

			ParserResult<slice>	res = as_slice(many(alt(
					as_slice(map(Location(), Server::with_location)),
					as_slice(map(directive(Listen()), Server::with_addr)),
					as_slice(map(directive(ServerName()), Server::name)),
					as_slice(map(directive(Root()), Server::root)),
					as_slice(map(directive(Cgi()), Server::cgi)),
					as_slice(map(directive(ClientMaxBodySize()), Server::max_size)),
					as_slice(map(directive(Index()), Server::index)),
					alt(
						as_slice(map(directive(ErrorPage()), Server::error_codes)),
						as_slice(map(comment, Server::dump)),
						as_slice(map(empty_line, Server::dump))
					)
					)))(input);
			return res.map(cfg);
		}
	};

	/*
	 * A create the server block
	 */
	class ServerBlock : public Parser<Server>
	{
	public:
		ServerBlock() { }

		result_type operator()(const slice &input)
		{
			ParserResult<tuple<slice, Server> >	res = Bloc<Tag, ServerContent>(Tag("server"), ServerContent())(input);
			if (res.is_err())
				return res.convert<Server>();
			return res.map(res.unwrap().second);
		}
	};
}


// TODO: handle comments and wrap up file (newline or eof at the end)
class ConfigParser : public Parser<std::vector<config::Server> >
{
public :
	ConfigParser() { }

	result_type operator()(const slice &input)
	{
		return terminated(many(
				delimited(many(newline, true),
				fail(config::ServerBlock()),
				many(newline, true))),
			sequence(take_with(Comment(), true), take_with(Newline(), true), Eof()))(input);
	}
};

#endif //WEBSERV_CONFIG_HPP
