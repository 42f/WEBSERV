//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_TAKEEXACT_HPP
#define WEBSERV_TAKEEXACT_HPP

#include "Parser.hpp"

/*
 * Matches everything until given string
 * Eg: TakeUntil("123") ("Hello world123")
 *  => result: Ok("Hello world", left : "123")
 */
class TakeExact: public Parser<slice>
{
private:
	size_t		_len;

public:
	TakeExact(size_t len);

	result_type operator()(const slice &input);
};

#endif //WEBSERV_TAKEEXACT_HPP
