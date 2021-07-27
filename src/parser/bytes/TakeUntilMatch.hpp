//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_TAKEUNTILMATCH_HPP
#define WEBSERV_TAKEUNTILMATCH_HPP

#include "Parser.hpp"

/*
 * Matches everything until given string
 * Eg: TakeUntilMatch(Tag("#")) ("Hello world#")
 *  => result: Ok("Hello world", left : "#")
 */
template<typename P>
class TakeUntilMatch: public Parser<slice>
{
private:
	P		_p;
	bool 	_empty;

public:
	TakeUntilMatch(P parser, bool empty): _p(parser), _empty(empty) { }

	result_type operator()(const slice &input)
	{
		slice	res = input.take(0);
		slice	left = input.from(0);

		while (left.size && _p(left).is_err())
		{
			res = input.take(res.size + 1);
			left = input.from(res.size);
		}
		if (!left.size && !_empty)
			return result_type::err(input, error("TakeUntilMatch: match found first"));
		return result_type::ok(left, res);
	}
};

template<typename P>
TakeUntilMatch<P>	take_until_match(P parser, bool empty = false)
{
	return TakeUntilMatch<P>(parser, empty);
}

#endif //WEBSERV_TAKEUNTILMATCH_HPP
