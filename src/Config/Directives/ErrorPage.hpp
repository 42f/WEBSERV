//
// Created by alena on 10/07/2021.
//

#ifndef WEBSERV_ERRORPAGE_HPP
#define WEBSERV_ERRORPAGE_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

/*
 * ErrorPage = error_page RWS isdigit [ isdigit ] RWS path
 */
class ErrorPage : public Parser<std::vector<std::pair<int, std::string> > >
{
public :
	ErrorPage();

	result_type operator()(const slice &input);
};

#endif //WEBSERV_ERRORPAGE_HPP
