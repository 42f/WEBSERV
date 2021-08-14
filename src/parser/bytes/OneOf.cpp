//
// Created by alena on 14/07/2021.
//

#include "OneOf.hpp"

/*
 * Matches a single character in the given string
 * Eg: OneOf("123456") ("123456")
 *  => result: Ok("1", left : 23456)
 */
OneOf::OneOf(std::string s): _m(s) { }

OneOf::result_type		OneOf::operator()(const slice& input)
{
	if (input.size == 0)
		return result_type::fail(input, error("incomplete", status::Incomplete));
	if (input.size && _m.find(*input.p) != std::string::npos)
		return result_type::ok(input.from(1), *input.p);
	return result_type::err(input, error("OneOf: no match in |" + _m + "|"));
}