//
// Created by alena on 23/06/2021.
//

#ifndef WEBSERV_ASSLICE_HPP
#define WEBSERV_ASSLICE_HPP

#include "Parser.hpp"

/*
 * Takes a parser, applies it, then returns the slice covering the parsed part
 */
template<typename P>
class AsSlice: public Parser<slice>
{
private:
	P		_p;

public:
	AsSlice(P p): _p(p) { }

	ParserResult<slice>	operator()(const slice&input)
	{
		typename P::result_type		res = _p(input);
		if (res.is_err())
			return res.template convert<slice>();
		slice left = res.left();
		return ParserResult<slice>::ok(left, slice(input.p, left.p - input.p));
	}
};

template<typename P>
AsSlice<P>	as_slice(P parser)
{
	return AsSlice<P>(parser);
}

#endif //WEBSERV_ASSLICE_HPP
