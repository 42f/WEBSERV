//
// Created by alena on 14/07/2021.
//

#include "TakeUntil.hpp"

/*
 * Matches everything until given string
 * Eg: TakeUntil("123") ("Hello world123")
 *  => result: Ok("Hello world", left : "123")
 */
TakeUntil::TakeUntil(const std::string &m): _m(m) { }

TakeUntil::result_type	TakeUntil::operator()(const slice &input)
{
	slice	res = input.take(0);
	slice	left = input.from(0);

	while (left.size && left.contains(_m))
	{
		res = input.take(res.size + 1);
		left = input.from(res.size);
	}
	if (!left.size)
		return result_type::err(input, error("TakeUntil: no match for |" + _m +"|"));
	return result_type::ok(left, res);
}