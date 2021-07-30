//
// Created by alena on 30/07/2021.
//

#ifndef WEBSERV_REQUESTPARSER_HPP
#define WEBSERV_REQUESTPARSER_HPP

#include "parser/export.hpp"
#include "Tokens.hpp"
#include "Request.hpp"

/*
 * RequestParser
 */
class RequestParser: public Parser<Request>
{
public:
	RequestParser();

	result_type operator()(const slice& input);
};

#endif //WEBSERV_REQUESTPARSER_HPP
