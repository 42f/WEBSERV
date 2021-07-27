//
// Created by alena on 14/07/2021.
//

#include "Alpha.hpp"

/*
 * Matches a single alphabetic character
 * Eg: Alpha("A") ("AbC")
 *  => result: Ok("A", left : bc)
 */
Alpha::Alpha() { }

Alpha::result_type		Alpha::operator()(const slice& input)
{
	if (std::isalpha(*input.p))
		return result_type::ok(input.from(1), *input.p);
	return result_type::err(input, error("Alpha: no match"));
}