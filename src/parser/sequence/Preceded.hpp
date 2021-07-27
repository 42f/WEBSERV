//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_PRECEDED_HPP
#define WEBSERV_PRECEDED_HPP

#include "Parser.hpp"

/*
 * Takes two parsers, applies them in sequence, return the result from the second if the first succeeded
 * Eg: Preceded(Tag("Hello"), Tag("World"))("HelloWorld")
 *  => result: Ok("World")
 */

template<typename P1, typename P2>
class Preceded: public Parser<typename P2::data_type>
{
private:
	P1		_p1;
	P2		_p2;

public:
	typedef typename P1::result_type	base_type;
	typedef typename P2::result_type	result_type;
	typedef typename P2::data_type		data_type;

	Preceded(P1 p1, P2 p2): _p1(p1), _p2(p2) { }

	result_type operator()(const slice& input)
	{
		base_type	res = _p1(input);
		if (res.is_err())
			return res.unwind(input, "Preceded: first item missing").template convert<data_type>();
		return _p2(res.left());
	}
};

template<typename P1, typename P2>
Preceded<P1, P2>		preceded(P1 p1, P2 p2)
{
	return Preceded<P1, P2>(p1, p2);
}

#endif //WEBSERV_PRECEDED_HPP
