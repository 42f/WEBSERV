//
// Created by alena on 06/07/2021.
//

#ifndef WEBSERV_CONFIG_SERVER_HPP
#define WEBSERV_CONFIG_SERVER_HPP

class LocationConfig;
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
		bool								_auto_index;
		std::vector<LocationConfig>			_locations;
		std::map<int, std::string>			_error_pages;
		std::map<std::string, std::string>	_cgis;

		static Server		*active;

	public:
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
		Server();

		static Server	invalid(slice input);

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
		static Server	*auto_index(bool auto_index);
		static Server	*with_location(LocationConfig config);

/*
** --------------------------------- GETTERS ----------------------------------
*/
		int									get_port() const;
		std::string const&					get_address() const;
		std::string const&					get_name() const;
		std::string &						get_root();
		std::string const&					get_root() const;
		std::string const&					get_index() const;
		bool const&							has_auto_index() const;
		size_t								get_body_size() const;
		std::vector<LocationConfig>&		get_locations();
		std::vector<LocationConfig> const&	get_locations() const;
		std::map<int, std::string>	const&	get_error_pages() const; //AVOIR
		std::map<std::string, std::string> const & get_cgis() const;

/*
** --------------------------------- METHODS ----------------------------------
*/
		static void	use(Server *ptr);

		Result<std::string>			error_page(int status_code);
		Result<std::string>			cgi_binary(std::string ext);

		bool 	is_invalid();
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
