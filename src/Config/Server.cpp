//
// Created by alena on 06/07/2021.
//
#include "../utils/Logger.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

LocationConfig *::LocationConfig::active = 0;

namespace config
{
	Server *Server::active = 0;

/*
 * ServerConfig
 */
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
	Server::Server() : _port(80), _address("0.0.0.0"), _name(""), _body_size(1048576) { }

	Server Server::invalid(slice input) {
		static_cast<void>(input);
		Server server;
		server._port = -1;
		return server;
	}
/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

/*
** --------------------------------- SETTERS ----------------------------------
*/
	Server *Server::with_addr(tuple<std::string, int> addr) {
		active->_address = addr.first;
		active->_port = addr.second;
		return active;
	}

	Server *Server::dump(slice unused) {
		(void)unused;
		return active;
	}

	Server *Server::name(slice name) {
		active->_name = name.to_string();
		return active;
	}

	Server *Server::max_size(size_t max_size) {
		active->_body_size = max_size;
		return active;
	}

	Server *Server::root(slice root) {
		active->_root = root.to_string();
		return active;
	}

	Server *Server::cgi(tuple<slice, slice> cgi) {
		active->_cgis.insert(std::make_pair(cgi.first.to_string(), cgi.second.to_string()));
		return active;
	}

	Server *Server::error_codes(std::vector<std::pair<int, std::string> > errors) {
		for (std::vector<std::pair<int, std::string> >::iterator it = errors.begin();
			 it != errors.end(); it++)
			active->_error_pages.insert(*it);
		return active;
	}

	Server *Server::index(slice index) {
		active->_index = index.to_string();
		return active;
	}

	Server *Server::with_location(LocationConfig config) {
		active->_locations.push_back(config);
		return active;
	}

/*
** --------------------------------- GETTERS ----------------------------------
*/
		int									Server::get_port() const { return _port; }
		std::string const&					Server::get_address() const { return _address; }
		std::string const&					Server::get_name() const { return _name; }
		std::string &						Server::get_root() { return _root; }
		std::string const&					Server::get_root() const { return _root; }
		std::string const&					Server::get_index() const { return _index; }
		size_t								Server::get_body_size() const { return _body_size; }
		std::vector<LocationConfig>&		Server::get_locations() { return _locations; }
		std::vector<LocationConfig> const&	Server::get_locations() const { return _locations; }
		std::map<int, std::string> const&	Server::get_error_pages() const { return _error_pages; }
		std::map<std::string, std::string> const & Server::get_cgis() const { return _cgis; };

/*
** --------------------------------- METHODS ----------------------------------
*/
	void Server::use(Server *ptr) { Server::active = ptr; }

	Result<std::string>	Server::cgi_binary(std::string ext)
	{
		std::map<std::string, std::string>::iterator it = this->_cgis.find(ext);
		if (it == this->_cgis.end())
			return Result<std::string>::err(DefaultError("Unknown cgi"));
		return Result<std::string>::ok(it->second);
	}

	bool Server::is_invalid() {
		return _port == -1;
	}
/*
** --------------------------------- OVERLOAD ---------------------------------
*/
	std::ostream &operator<<(std::ostream &stream, const Server &cfg)
	{
		stream << YELLOW << "Server {" << NC << std::endl
				<< "AdresseIP/Port : " << cfg._address << ":" << cfg._port << std::endl
				<< "Server_name : " << cfg._name << std::endl
				<< "Root : " << cfg._root << std::endl
				<< "Index : " << cfg._index << std::endl
				<< "Client_max_body_size : " << cfg._body_size << " bytes" << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = cfg._cgis.begin();
			 it != cfg._cgis.end(); it++) {
			stream << "CGI : " << it->first << " -> " << it->second << std::endl;
		}
		for (std::map<int, std::string>::const_iterator it = cfg._error_pages.begin();
			 it != cfg._error_pages.end(); it++) {
			stream << "error_page: " << it->first << " -> " << it->second << std::endl;
		}
		stream << std::endl;
		for (std::vector<LocationConfig>::const_iterator it = cfg._locations.begin();
			 it != cfg._locations.end(); it++) {
			stream << *it << std::endl;
		}
		stream << YELLOW << "}" << NC << std::endl;
		return stream;
	}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/
	std::vector<Server> parse(const std::string &path)
	{
		struct stat tmpBuff;
 	 	if (stat(path.c_str(), &tmpBuff) != 0) {
			  std::cerr << RED;
			  perror(("Error reading " + path).c_str());
			  std::cerr << NC;
			  exit(1);
		}
		std::ifstream t(path.c_str());
		std::stringstream buffer;
		buffer << t.rdbuf();
		std::string		str = buffer.str();
		slice cfg(str);

		cfg.size++; // Make sure we parse the \0 at the end.
		ParserResult<std::vector<config::Server> >	cfgs = ConfigParser()(cfg);
		if (cfgs.is_err())
		{
			{
				LogStream stream; stream << cfgs.unwrap_err();
#if LOG_LEVEL == LOG_LEVEL_TRACE	//start line error
				cfgs.unwrap_err().trace(cfg, stream);
#endif
				std::cerr << RED << "Configuration error" << NC << std::endl;
			}
			exit(-1);
		} else
		{
			LogStream() << "Parsed " << cfgs.unwrap().size() << " servers\n";
			for (std::vector<config::Server>::iterator it = cfgs.unwrap().begin(); it != cfgs.unwrap().end(); it++) {
				std::cout << *it << std::endl;
			}//TODO remove for
			return cfgs.unwrap();
		}
	}
}

/* ************************************************************************** */
