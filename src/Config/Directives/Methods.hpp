//
// Created by alena on 08/07/2021.
//

#ifndef WEBSERV_CONFIG_METHODS_HPP
#define WEBSERV_CONFIG_METHODS_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"
#include "HTTP/Request/RequestLine.hpp"


/*
 * Method = method GET POST ...
 * sequence(slice, many(preceded(Char(" "), slice))
 */
namespace config
{
	class Methods : public Parser<methods::Methods>
	{
	public :
		Methods();

		result_type operator()(const slice &input);
	};
}

/* ************************************************************************** */

#endif //WEBSERV_METHODS_HPP
