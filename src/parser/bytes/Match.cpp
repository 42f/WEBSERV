//
// Created by alena on 14/07/2021.
//

#include "Match.hpp"

/*
 * Matches a single character from the given function
 * Eg: Match(isdigit) ("1HelloWorld")
 *  => result: Ok("1", left : HelloWorld)
 */
Match::Match(match_fn fn) : _fn(fn) { }

Match::result_type		Match::operator()(const slice& input)
{
	if (_fn(*input.p))
		return result_type::ok(input.from(1), *input.p);
	return result_type::err(input, error("Match: no match for |" + std::string(1, *input.p) + "|"));
}

namespace streaming {

	Match::Match(match_fn fn) : _fn(fn) { }

	Match::result_type		Match::operator()(const slice& input)
	{
		if (input.size == 0)
			return result_type::fail(input, error("incomplete", status::Incomplete));
		if (_fn(*input.p))
			return result_type::ok(input.from(1), *input.p);
		return result_type::err(input, error("Match: no match for |" + std::string(1, *input.p) + "|"));
	}
}