//
// Created by alena on 06/07/2021.
//

#ifndef WEBSERV_SERVERNAME_HPP
#define WEBSERV_SERVERNAME_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

/*
 * ServerName = server_name RWS print
 */
class ServerName : public Parser<slice>
{
public :
	ServerName();

	result_type operator()(const slice &input);
};


#endif //WEBSERV_SERVERNAME_HPP
