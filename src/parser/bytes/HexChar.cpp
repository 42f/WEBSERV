//
// Created by alena on 14/07/2021.
//

#include "HexChar.hpp"

/*
 * Matches a single character in the given hex range
 * Eg: HexChar("H") ("HelloWorld")
 *  => result: Ok("H", left : elloWorld)
 */
HexChar::HexChar(int low, int high): _l(low), _h(high) { }

HexChar::result_type	HexChar::operator()(const slice& input)
{
	if (*input.p >= _l && *input.p <= _h)
		return result_type::ok(input.from(1), *input.p);
	return result_type::err(input, error("Char: no match for |" + std::string(1, *input.p) + "|"));
}

namespace streaming {
	HexChar::HexChar(int low, int high): _l(low), _h(high) { }

	HexChar::result_type	HexChar::operator()(const slice& input)
	{
		if (input.size == 0)
			return result_type::fail(input, error("incomplete", status::Incomplete));
		if (*input.p >= _l && *input.p <= _h)
			return result_type::ok(input.from(1), *input.p);
		return result_type::err(input, error("Char: no match for |" + std::string(1, *input.p) + "|"));
	}
}