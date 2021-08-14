//
// Created by alena on 14/07/2021.
//

#include "Digit.hpp"

/*
 * Matches a single digit
 * Eg: Digit("1") ("123")
 *  => result: Ok("1", left : 23)
 */
Digit::Digit() { }

Digit::result_type		Digit::operator()(const slice& input)
{
	if (input.size == 0)
		return result_type::fail(input, error("incomplete", status::Incomplete));
	if (std::isdigit(*input.p))
		return result_type::ok(input.from(1), *input.p);
	return result_type::err(input, error("Digit: no match"));
}