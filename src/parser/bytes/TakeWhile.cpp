//
// Created by alena on 14/07/2021.
//

#include "TakeWhile.hpp"
/*
 * Matches everything that the predicate returns true for
 * Eg: TakeWhile(isdigit) ("12345Hello")
 *  => result: Ok("12345", left : "Hello")
 * For infinite maximum, set to -1
 */
TakeWhile::TakeWhile(match_fn fn, bool empty) : _fn(fn), _empty(empty), _min(empty ? 0 : 1), _max(-1) { }
TakeWhile::TakeWhile(match_fn fn, int min, int max) : _fn(fn), _empty(min == 0), _min(min), _max(max) { }

TakeWhile::result_type	TakeWhile::operator()(const slice &input)
{
	slice	res = input.take(0);
	slice	left = input.from(0);
	int 	n = _max;

	if (input.size == 0)
		return result_type::fail(input, error("incomplete", status::Incomplete));
	while (left.size && _fn(*left.p) && n--)
	{
		res = input.take(res.size + 1);
		left = input.from(res.size);
	}
	if (res.size == 0 && !_empty)
		return result_type::err(input, error("TakeWhile: no match"));
	if (res.size && (n > 0 || static_cast<int>(res.size) < _min))
		return result_type::err(input, error("TakeWhile: not enough matches"));
	return result_type::ok(left, res);
}