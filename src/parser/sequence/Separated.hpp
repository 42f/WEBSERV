//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_SEPARATED_HPP
#define WEBSERV_SEPARATED_HPP

#include "Parser.hpp"
#include "Error.hpp"

/*
 * Get an object from P1, then gets an object from S and discard it, then gets an object from P2
 * Eg: Separated(Tag("Hello"), Char(' '), Tag("World"))("Hello World")
 *  => result: Ok(tuple("Hello", "World"), left = "")
 */

template<typename P1, typename S, typename P2>
class Separated: public Parser<tuple<typename P1::data_type, typename P2::data_type> >
{
private:
P1	_p1;
P2	_p2;
S	_s;

typedef typename P1::data_type						first_type;
typedef typename P2::data_type						second_type;

typedef typename S::result_type						separator_type;
typedef typename P1::result_type					first_result;
typedef typename P2::result_type					second_result;

public:
typedef ParserResult<tuple<first_type, second_type> >	result_type;

Separated(P1 p1, S s, P2 p2): _p1(p1), _p2(p2), _s(s) { }

result_type	operator()(const slice& input)
{
	first_result	r1 = _p1(input);
	if (r1.is_err())
		return result_type::err(input, error("Separated: not found"));
	separator_type r = _s(r1.left());
	if (r.is_err() && r.left() != r1.left())
		return r.failure().unwind(r1.left(), "Separated: bad separator").template convert<tuple<first_type, second_type> >();
	else if (r.is_err())
		return r.unwind(r1.left(), "Separated: missing separator").template convert<tuple<first_type, second_type> >();
	second_result	r2 = _p2(r.left());
	if (r2.is_err())
		return (r2.failure().template convert<tuple<first_type, second_type> >().unwind(r.left(), "Separated: missing token"));
	return result_type::ok(r2.left(), tuple<first_type, second_type>(r1.unwrap(), r2.unwrap()));
}
};

template<typename P1, typename S, typename P2>
Separated<P1, S, P2>	separated(P1 p1, S s, P2 p2)
{
	return Separated<P1, S, P2>(p1, s, p2);
}

#endif //WEBSERV_SEPARATED_HPP
