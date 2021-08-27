//
// Created by alena on 11/07/2021.
//

#ifndef WEBSERV_REDIRECT_HPP
#define WEBSERV_REDIRECT_HPP

#include <string>

#include "HTTP/Request/RequestUtils/Target.hpp"
#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

struct redirect
{
	status::StatusCode	status;
	std::string			uri;

	redirect();
	redirect(int code, std::string uri);
	redirect(status::StatusCode code, std::string uri);

	static redirect	parse(tuple<slice, slice> input);
	std::string resolveRedirect(Target const& target) const;
};

/*
 * Redirect = return digit path
 */
class Redirect : public Parser<redirect>
{
public :
	Redirect();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */


#endif //WEBSERV_REDIRECT_HPP
