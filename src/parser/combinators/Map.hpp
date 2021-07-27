//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_MAP_HPP
#define WEBSERV_MAP_HPP

#include "Parser.hpp"

/*
 * Takes a parser, apply it then convert the result using the supplied function
 * Typically, build a structure from its raw parts stored in a tuple
 * Eg: Map(Separated(Digit(), Char(.), Digit()), version::from_tuple)("1.1 version")
 *  => result: Ok(Version(1.1), left :" version")
 */
template<typename P, typename T2>
class Map: public Parser<T2>
{
private:
	typedef typename P::result_type		base_type;
	typedef	 T2							(*fn_type)(typename P::data_type);

	P			_parser;
	fn_type		_fn;

public:
	typedef typename Parser<T2>::result_type	result_type;

	Map(P parser, fn_type map): _parser(parser), _fn(map) { }

	result_type operator()(const slice &input)
	{
		base_type	res = _parser(input);
		if (res.is_err())
			return res.template convert<T2>().unwind(input, "Map");
		return result_type::ok(res.left(), _fn(res.unwrap()));
	}
};

template<typename P, typename T2>
Map<P, T2> map(P parser, T2 (*fn)(typename P::data_type))
{
	return Map<P, T2>(parser, fn);
}

#endif //WEBSERV_MAP_HPP
