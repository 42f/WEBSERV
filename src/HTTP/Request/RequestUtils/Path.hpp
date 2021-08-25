//
// Created by alena on 18/06/2021.
//

#ifndef WEBSERV_PATH_HPP
#define WEBSERV_PATH_HPP

#include "Parser.hpp"
#include "Tokens.hpp"
#include "Charset.hpp"

#include "Target.hpp"

/*
 * AbsolutePath = 1*( "/" segment )
 */

class AbsolutePath : public Parser<std::string>
{
public :

	AbsolutePath();

	result_type	operator()(const slice &input);
};

/* ************************************************************************** */

/*
 * Query = *( pchar / "/" / "?" )
 */
class Query: public Parser<slice>
{
public :
	Query();

	result_type	operator()(const slice &input);
};

/* ************************************************************************** */

/*
 * OriginForm = AbsolutePath [ "?" Query ]
 */
class OriginForm : public Parser<Target>
{
public :
	OriginForm();

	result_type	operator()(const slice &input);
};

/* ************************************************************************** */


#endif //WEBSERV_PATH_HPP
