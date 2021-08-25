//
// Created by alena on 06/07/2021.
//

#ifndef WEBSERV_ROOT_HPP
#define WEBSERV_ROOT_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"
#include "HTTP/Request/RequestUtils/Path.hpp"

#include "sys/stat.h"

/*
 * Root = root RWS path
 */
class Root : public Parser<slice>
{
public :
	Root();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */

#endif //WEBSERV_ROOT_HPP
