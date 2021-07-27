//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_TAKEUNTIL_HPP
#define WEBSERV_TAKEUNTIL_HPP

#include "Parser.hpp"

/*
 * Matches everything until given string
 * Eg: TakeUntil("123") ("Hello world123")
 *  => result: Ok("Hello world", left : "123")
 */
class TakeUntil: public Parser<slice>
{
private:
	std::string		_m;

public:
	TakeUntil(const std::string &m);

	result_type operator()(const slice &input);
};

#endif //WEBSERV_TAKEUNTIL_HPP
