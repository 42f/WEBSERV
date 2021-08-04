//
// Created by alena on 08/07/2021.
//

#ifndef WEBSERV_CLIENTMAXBODYSIZE_HPP
#define WEBSERV_CLIENTMAXBODYSIZE_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

/*
 * ClientMaxBodySize = client_max_body_size 100Mo
 */
class ClientMaxBodySize : public Parser<size_t>
{
public :
	ClientMaxBodySize();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */

#endif //WEBSERV_CLIENTMAXBODYSIZE_HPP
