//
// Created by alena on 14/07/2021.
//

#include "Char.hpp"

/*
 * Matches a single character
 * Eg: Char("H") ("HelloWorld")
 *  => result: Ok("H", left : elloWorld)
 */
Char::Char(char c): _m(c) { }

Char::result_type		Char::operator()(const slice& input)
{
	if (input.size == 0)
		return result_type::fail(input, error("incomplete", status::Incomplete));
	if (*input.p == _m)
		return result_type::ok(input.from(1), _m);
	return result_type::err(input, error("Char: no match for |" + std::string(1, _m) + "|"));
}
