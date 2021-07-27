//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_DELIMITED_HPP
#define WEBSERV_DELIMITED_HPP

#include "Parser.hpp"
#include "Error.hpp"

/*
 * Get an object from P1 and discard it, then gets an object from S, then gets an object from P2 and discard it
 * Eg: Delimited(Tag("Hello"), Char(' '), Tag("World"))("Hello World")
 *  => result: Ok(" ", left = "")
 */

template<typename P1, typename S, typename P2>
class Delimited: public Parser<typename S::data_type>
{
private:
	P1	_p1;
	P2	_p2;
	S	_s;

	typedef typename P1::data_type						first_type;
	typedef typename P2::data_type						second_type;

	typedef typename P1::result_type					first_result;
	typedef typename P2::result_type					second_result;

public:
	typedef typename S::result_type						result_type;

	Delimited(P1 p1, S s, P2 p2): _p1(p1), _p2(p2), _s(s) { }

	result_type	operator()(const slice& input)
	{
		first_result	r1 = _p1(input);
		if (r1.is_err())
			return r1.template convert<typename S::data_type>().unwind(input, "Delimited: not found");
		result_type r = _s(r1.left());
		if (r.is_err())
			return r.unwind(r1.left(), "Delimited: missing content");
		second_result	r2 = _p2(r.left());
		if (r2.is_err()) {
			result_type res = r2.template convert<typename S::data_type>().unwind(r.left(), "Delimited: missing end");
			return r.left().p != input.p ? res.failure() : res;
		}
		return result_type::ok(r2.left(), r.unwrap());
	}
};

template<typename P1, typename S, typename P2>
Delimited<P1, S, P2>	delimited(P1 p1, S s, P2 p2)
{
	return Delimited<P1, S, P2>(p1, s, p2);
}

#endif //WEBSERV_DELIMITED_HPP
