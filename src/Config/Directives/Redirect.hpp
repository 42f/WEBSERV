//
// Created by alena on 11/07/2021.
//

#ifndef WEBSERV_REDIRECT_HPP
#define WEBSERV_REDIRECT_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

struct redirect {
	unsigned int	status;
	std::string		uri;

	redirect();
	redirect(int code, std::string uri);

	static redirect	parse(tuple<slice, slice> input);
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

#endif //WEBSERV_REDIRECT_HPP
