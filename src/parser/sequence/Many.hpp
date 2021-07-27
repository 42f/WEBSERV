//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_MANY_HPP
#define WEBSERV_MANY_HPP

#include "Parser.hpp"
#include <vector>

/*
 * Take parser, applies it as many times as possible, push the results in a vector
 * Eg: Many(Tag("A"))("AAA")
 *  => result: Ok(["A", "A", "A"])
 * Eg: Many(Tag("A"))("AABA")
 *  => result: Ok(["A", "A"], left: "BA")
 */
template<typename P>
class Many: public Parser<std::vector<typename P::data_type> >
{
private:
	P		_p;
	bool	_empty_ok;

public:
	typedef typename std::vector<typename P::data_type>	data_type;
	typedef typename P::result_type						base_type;
	typedef ParserResult<data_type>							result_type;

	Many(P parser, bool empty_ok = true): _p(parser), _empty_ok(empty_ok) { }

	result_type 	operator()(const slice& input)
	{
		data_type 		vec;
		base_type		res = _p(input);
		slice			left = input;

		while (res.is_ok())
		{
			left = res.left();
			vec.push_back(res.unwrap());
			res = _p(res.left());
		}
		if (res.is_failure())
			return res.template convert<data_type>().unwind(input, "Many: failure while parsing");
		if (vec.empty() && !_empty_ok)
			return res.template convert<data_type>().unwind(input, "Many: no match");
		return result_type::ok(left, vec);
	}
};

template<typename P>
Many<P>		many(P parser, bool empty_ok = false)
{
	return Many<P>(parser, empty_ok);
}

#endif //WEBSERV_MANY_HPP
