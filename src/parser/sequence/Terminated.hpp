//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_TERMINATED_HPP
#define WEBSERV_TERMINATED_HPP

#include "Parser.hpp"

/*
 * Takes two parsers, applies them in sequence, return the result from the first if the second succeeded
 * Eg: Terminated(Tag("Hello"), Tag("World"))("HelloWorld")
 *  => result: Ok("Hello")
 */

template<typename P1, typename P2>
class Terminated: public Parser<typename P1::data_type>
{
private:
	P1		_p1;
	P2		_p2;

public:
	typedef typename P2::result_type	base_type;
	typedef typename P1::result_type	result_type;

	Terminated(P1 p1, P2 p2): _p1(p1), _p2(p2) { }

	result_type operator()(const slice& input)
	{
		result_type 	res = _p1(input);
		if (res.is_err())
			return res.unwind(input, "Terminated: no match for first");
		base_type		r = _p2(res.left());
		if (r.is_err())
			return r.template convert<typename P1::data_type>().unwind(input, "Terminated: no match for second");
		return result_type::ok(r.left(), res.unwrap());
	}
};

template<typename P1, typename P2>
Terminated<P1, P2>		terminated(P1 p1, P2 p2)
{
	return Terminated<P1, P2>(p1, p2);
}

#endif //WEBSERV_TERMINATED_HPP
