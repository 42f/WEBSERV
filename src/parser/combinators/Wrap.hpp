//
// Created by alena on 06/07/2021.
//

#ifndef WEBSERV_WRAP_HPP
#define WEBSERV_WRAP_HPP

#include "Parser.hpp"

template<typename P>
class WrapError: public Parser<typename P::data_type>
{
private:
	P				_parser;
	std::string 	_ctx;

public:
	WrapError(P parser, std::string context): _parser(parser), _ctx(context) { }

	typename P::result_type		operator()(const slice &input)
	{
		typename P::result_type res = _parser(input);
		if (res.is_err())
			return res.unwind(input, _ctx);
		return res;
	}
};

template<typename P>
WrapError<P>	wrap_error(std::string context, P parser) { return WrapError<P>(parser, context); }

template<typename P>
class Debug: public Parser<typename P::data_type> {
private:
	std::string _ctx;
	P			_parser;
public:
	Debug(std::string context, P parser): _ctx(context), _parser(parser) {}

	typename P::result_type		operator()(const slice &input)
	{
		typename P::result_type	res = _parser(input);
		std::cout << "[" << _ctx << "] " << res << std::endl;
		return res;
	}
};

template<typename P>
Debug<P>	debug(std::string context, P parser) { return Debug<P>(context, parser); }

template<typename P>
class Fail: public Parser<typename P::data_type> {
private:
	P	_parser;

public:
	Fail(P parser): _parser(parser) { }

	typename Fail::result_type operator()(const slice &input) {
		typename Fail::result_type res = _parser(input);
		if (res.left() != input)
			return res.failure();
		return res;
	}
};

template<typename P>
Fail<P>		fail(P parser) { return Fail<P>(parser); }

#endif //WEBSERV_WRAP_HPP
