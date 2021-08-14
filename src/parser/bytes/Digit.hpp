//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_DIGIT_HPP
#define WEBSERV_DIGIT_HPP

#include "Parser.hpp"

/*
 * Matches a single digit
 * Eg: Digit("1") ("123")
 *  => result: Ok("1", left : 23)
 */
class Digit: public Parser<char>
{
public:
	typedef Parser::result_type		result_type;

	Digit();

	result_type		operator()(const slice& input);

};

const Digit	digit = Digit();

#endif //WEBSERV_DIGIT_HPP
