//
// Created by alena on 28/07/2021.
//

#ifndef WEBSERV_CONFIGPARSER_HPP
#define WEBSERV_CONFIGPARSER_HPP

#include "ServerBlock.hpp"

class ConfigParser : public Parser<std::vector<config::Server> >
{
public :
	ConfigParser();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */


#endif //WEBSERV_CONFIGPARSER_HPP
