//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_MATCH_HPP
#define WEBSERV_MATCH_HPP

#include "Parser.hpp"

/*
 * Matches a single character from the given function
 * Eg: Match(isdigit) ("1HelloWorld")
 *  => result: Ok("1", left : HelloWorld)
 */
class Match: public Parser<char>
{
private:
	typedef int		(*match_fn)(int c);

private:
	match_fn 	_fn;

public:
	Match(match_fn fn);

	result_type		operator()(const slice& input);
};

namespace streaming {
	class Match: public Parser<char>
	{
	private:
		typedef int		(*match_fn)(int c);

	private:
		match_fn 	_fn;

	public:
		Match(match_fn fn);

		result_type		operator()(const slice& input);
	};

}

#endif //WEBSERV_CHAR_HPP
