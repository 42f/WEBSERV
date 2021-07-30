//
// Created by alena on 06/07/2021.
//

#include "Server.hpp"

LocationConfig *::LocationConfig::active = 0;

namespace config
{
	Server *Server::active = 0;

/*
 * ServerConfig
 */
	Server::Server() : _port(0), _address(""), _name(""), _body_size(1048576) { }

	void Server::use(Server *ptr) { Server::active = ptr; }

/*
 * SETTER
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
 * GETTER
 */
	unsigned short	Server::get_port() { return _port; }
	std::string		Server::get_addr() { return _address; }
	std::string		Server::get_name() { return _name; }
/*
 * RESPONSE
 */

/*
 * CGI_BINARY
 */
	Result<std::string>	Server::cgi_binary(std::string ext)
	{
		std::map<std::string, std::string>::iterator it = this->_cgis.find(ext);
		if (it == this->_cgis.end())
			return Result<std::string>::err(DefaultError("Unknown cgi"));
		return Result<std::string>::ok(it->second);
	}

	std::ostream &operator<<(std::ostream &stream, const Server &cfg)
	{
		stream << YELLOW << "AdresseIP/Port : " << cfg._address << ":" << cfg._port << std::endl
			   << "Server_name : " << cfg._name << std::endl
			   << "Root : " << cfg._root << std::endl
			   << "Client_max_body_size : " << cfg._body_size << " bytes" << NC << std::endl << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = cfg._cgis.begin();
			 it != cfg._cgis.end(); it++) {
			stream << "CGI : " << it->first << " -> " << it->second << std::endl;
		}
		stream << std::endl;
		for (std::vector<LocationConfig>::const_iterator it = cfg._locations.begin();
			 it != cfg._locations.end(); it++) {
			stream << *it << std::endl;
		}
		for (std::map<int, std::string>::const_iterator it = cfg._error_pages.begin();
			 it != cfg._error_pages.end(); it++) {
			stream << "error_page: " << it->first << " -> " << it->second << std::endl;
		}
		return stream;
	}
}