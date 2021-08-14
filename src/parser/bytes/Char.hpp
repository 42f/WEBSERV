//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_CHAR_HPP
#define WEBSERV_CHAR_HPP

#include "Parser.hpp"

/*
 * Matches a single character
 * Eg: Char("H") ("HelloWorld")
 *  => result: Ok("H", left : elloWorld)
 */
class Char: public Parser<char>
{
private:
	char	_m;

public:
	typedef Parser::result_type		result_type;

	Char(char c);

	result_type		operator()(const slice& input);

};
#endif //WEBSERV_CHAR_HPP
