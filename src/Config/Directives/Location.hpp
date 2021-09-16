//
// Created by alena on 07/07/2021.
//

#ifndef WEBSERV_LOCATION_HPP
#define WEBSERV_LOCATION_HPP

#include "Config/Server.hpp"

namespace config {
	class Server;
}

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

#include "parsing/Methods.hpp"

#include "Config/Structure/Bloc.hpp"
#include "Config/Structure/Line.hpp"

#include "Root.hpp"
#include "Index.hpp"
#include "ClientMaxBodySize.hpp"
#include "Methods.hpp"
#include "Redirect.hpp"
#include "Upload.hpp"
#include "Network/ServerPool.hpp"


#include <limits>
namespace network {
	class ServerPool;
}

class LocationConfig
{
private:

	friend class network::ServerPool;

	std::string				_path;
	methods::Methods		_methods;
	bool					_auto_index;
	bool					_upload;
	size_t					_body_size;
	std::string				_root;
	std::string				_index;
	redirect				_redirect;

	static LocationConfig	*active;

	static const size_t		SIZE_UNSET;

public:
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
	LocationConfig();

	private:
	/*
	 * Allow to instanciate a location with all server data whenever a server
	 * has no location to be used by ResponseHandler::locationMatch()
	 */
	LocationConfig(config::Server const & parentServer );

	public:
/*
** --------------------------------- METHODS ----------------------------------
*/
	static	void	use(LocationConfig *ptr);
/*
** --------------------------------- GETTERS ----------------------------------
*/
	static	LocationConfig	*path(slice path);
	static	LocationConfig	*root(slice root);
	static	LocationConfig	*body_size(size_t size);
	static	LocationConfig	*auto_index(bool auto_index);
	static	LocationConfig	*upload(bool upload);
	static	LocationConfig	*methods(methods::Methods methods);
	static	LocationConfig	*index(slice index);
	static	LocationConfig	*redirection(redirect ret);
	static	LocationConfig	*dump(slice unused);

	std::string	const &		get_path() const;
	methods::Methods		get_methods() const;
	bool					has_auto_index() const;
	bool					get_upload() const;
	size_t					get_body_size() const;
	std::string				get_root() const;
	std::string				get_index() const;
	bool					has_index() const;
	redirect				get_redirect() const;
/*
** --------------------------------- SETTERS ----------------------------------
*/
	LocationConfig	&set_path(slice path);
	LocationConfig	&set_size(size_t size);
	LocationConfig	&set_root(slice root);
	LocationConfig	&set_autoindex(bool index);
	LocationConfig	&set_upload(bool upload);
	LocationConfig	&set_methods(methods::Methods methods);
	LocationConfig	&set_index(slice index);
	LocationConfig	&set_redirect(redirect ret);


/*
** --------------------------------- OVERLOAD ---------------------------------
*/
	friend std::ostream &operator<<(std::ostream& stream, const LocationConfig& cfg);
};

/* ************************************************************************** */

/*
 * LocationContent is a line for block server
 * listen | server_name ...
 */
class LocationContent: public Parser<LocationConfig>
{
public:
	LocationContent();

	result_type 	operator()(const slice &input);
};

/* ************************************************************************** */


class Head : public Parser<slice>
{
public:
	Head();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */

/*
 * Location = location RWS path RWS { . . . }
 */
class Location : public Parser<LocationConfig>
{
public:
	Location();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */

#endif //WEBSERV_LOCATION_HPP
