//
// Created by alena on 06/07/2021.
//

#ifndef WEBSERV_CONFIG_SERVER_HPP
#define WEBSERV_CONFIG_SERVER_HPP

#include "Config/Directives/Location.hpp"
#include "HTTP/Request/Request.hpp"
#include <map>

namespace config
{
/*
 * Result of server block
 */
	class Server
	{
	private:
		int									_port;
		std::string							_address;
		std::string							_name;
		std::string							_root;
		std::string							_index;
		size_t								_body_size;		// in bytes
		std::vector<LocationConfig>			_locations;
		std::map<int, std::string>			_error_pages;
		std::map<std::string, std::string>	_cgis;

		static Server		*active;

	public:
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
		Server();

/*
** --------------------------------- SETTERS ----------------------------------
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
** --------------------------------- GETTERS ----------------------------------
*/
		int									get_port();
		std::string							get_address();
		std::string							get_name();
		std::string							get_root();
		std::string							get_index();
		size_t								get_body_size();
		std::vector<LocationConfig>			get_locations();
		std::map<int, std::string>			get_error_pages(); //AVOIR

/*
** --------------------------------- METHODS ----------------------------------
*/
		static void	use(Server *ptr);

		Result<std::string>			error_page(int status_code);
		Result<std::string>			cgi_binary(std::string ext);

/*
** --------------------------------- OVERLOAD ---------------------------------
*/
		friend std::ostream &operator<<(std::ostream& stream, const Server& cfg);
	};
/*
** --------------------------------- ACCESSOR ---------------------------------
*/

	std::vector<Server>		parse(const std::string &path);
}

/* ************************************************************************** */


#endif //WEBSERV_SERVER_HPP
