//
// Created by alena on 14/07/2021.
//

#include "TakeExact.hpp"

/*
 * Matches exactly the given size
 * Eg: TakeExact("123") ("Hello world123")
 *  => result: Ok("Hello world", left : "123")
 */
TakeExact::TakeExact(size_t len): _len(len) { }

TakeExact::result_type	TakeExact::operator()(const slice &input)
{
	if (input.size < _len)
		return result_type::err(input, error("TakeExact: incomplete"));
	return result_type::ok(input.from(_len), input.take(_len));
}