//
// Created by alena on 28/07/2021.
//

#ifndef WEBSERV_SERVERBLOCK_HPP
#define WEBSERV_SERVERBLOCK_HPP

#include "Config/Directives/Cgi.hpp"
#include "Config/Directives/ClientMaxBodySize.hpp"
#include "Config/Directives/ErrorPage.hpp"
#include "Config/Directives/Listen.hpp"
#include "Config/Directives/ServerName.hpp"
#include "Config/Directives/Root.hpp"
#include "Config/Directives/Index.hpp"
#include "Config/Directives/Location.hpp"

#include "Server.hpp"
#include "Structure/Bloc.hpp"
#include "Structure/Line.hpp"

/*
 * ServerContent is a line for block server
 * listen | server_name ...
 */
class ServerContent : public Parser<config::Server>
{
public:
	ServerContent();

	result_type operator()(const slice &input);
};


/*
 * A create the server block
 */
class ServerBlock : public Parser<config::Server>
{
public:
	ServerBlock();

	result_type operator()(const slice &input);
};


#endif //WEBSERV_SERVERBLOCK_HPP
