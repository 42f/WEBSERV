//
// Created by alena on 20/06/2021.
//

#ifndef WEBSERV_OPT_HPP
#define WEBSERV_OPT_HPP

#include "Parser.hpp"

/*
 * Applies a parser, if it fails, returns ok anyway
 * Eg: Opt(Tag("Hello")) ("World")
 *  => result: Ok(left: "World")
 */
template<typename P>
class Opt: public Parser<typename P::data_type>
{
private:
	P	_parser;

public:
	typedef typename P::result_type	result_type;

	Opt(P parser): _parser(parser) { }

	result_type		operator()(const slice &input)
	{
		result_type		res = _parser(input);

		if (res.is_failure())
			return res;
		else if (res.is_err())
			return result_type::ok(input,typename P::data_type());
		return res;
	}

};

template<typename P>
Opt<P>	opt(P parser)
{
	return Opt<P>(parser);
}

#endif //WEBSERV_OPT_HPP
