//
// Created by alena on 08/07/2021.
//

#ifndef WEBSERV_INDEX_HPP
#define WEBSERV_INDEX_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

/*
 * Index = index index.html;
 */
class Index : public Parser<slice>
{
public :
	Index();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */

/*
 * AutoIndex = autoindex on | off
 */
class AutoIndex : public Parser<bool>
{
public :
	AutoIndex();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */

#endif //WEBSERV_INDEX_HPP
