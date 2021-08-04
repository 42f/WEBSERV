//
// Created by alena on 07/07/2021.
//

#ifndef WEBSERV_LOCATION_HPP
#define WEBSERV_LOCATION_HPP

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

#include <limits>

class LocationConfig
{
private:
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
	LocationConfig();

	static	void	use(LocationConfig *ptr);

/*
 * GETTER
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
/*
 * SETTER
 */
	LocationConfig	&set_path(slice path);
	LocationConfig	&set_size(size_t size);
	LocationConfig	&set_root(slice root);
	LocationConfig	&set_autoindex(bool index);
	LocationConfig	&set_upload(bool upload);
	LocationConfig	&set_methods(methods::Methods methods);
	LocationConfig	&set_index(slice index);
	LocationConfig	&set_redirect(redirect ret);

	friend std::ostream &operator<<(std::ostream& stream, const LocationConfig& cfg);
};

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

class Head : public Parser<slice>
{
public:
	Head();

	result_type operator()(const slice &input);
};

/*
 * Location = location RWS path RWS { . . . }
 */
class Location : public Parser<LocationConfig>
{
public:
	Location();

	result_type operator()(const slice &input);
};


#endif //WEBSERV_LOCATION_HPP
