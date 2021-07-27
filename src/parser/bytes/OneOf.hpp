//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_ONEOF_HPP
#define WEBSERV_ONEOF_HPP

#include "Parser.hpp"

/*
 * Matches a single character in the given string
 * Eg: OneOf("123456") ("123456")
 *  => result: Ok("1", left : 23456)
 */
class OneOf: public Parser<char>
{
private:
	std::string	_m;

public:
	typedef Parser::result_type		result_type;

	OneOf(std::string s);

	result_type		operator()(const slice& input);
};

#endif //WEBSERV_CHAR_HPP
