//
// Created by alena on 09/07/2021.
//

#ifndef WEBSERV_STRIP_HPP
#define WEBSERV_STRIP_HPP

#include <slice.hpp>

template<typename P>
class Strip: public Parser<slice>
{
private:
	P	_parser;

public:
	Strip(P parser): _parser(parser) {  }

	result_type		operator()(const slice& input)
	{
		typename P::result_type	res = _parser(input);

		if (res.is_err())
			return res.template convert<slice>();
		slice	parsed = input.take(res.left().p - input.p);
		while (std::isspace(*parsed.p))
		{
			parsed.p++;
			parsed.size--;
		}
		while (std::isspace(parsed.p[parsed.size - 1]))
			parsed.size--;
		return result_type::ok(res.left(), parsed);
	}
};

template<typename P>
Strip<P>	strip(P parser) { return Strip<P>(parser); }

#endif //WEBSERV_STRIP_HPP
