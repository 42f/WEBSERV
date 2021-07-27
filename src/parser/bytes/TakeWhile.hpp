//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_TAKEWHILE_HPP
#define WEBSERV_TAKEWHILE_HPP

#include "Parser.hpp"
#include "Error.hpp"
#include "slice.hpp"

/*
 * Matches everything that the predicate returns true for
 * Eg: TakeWhile(isdigit) ("12345Hello")
 *  => result: Ok("12345", left : "Hello")
 * For infinite maximum, set to -1
 */
class TakeWhile: public Parser<slice>
{
public:
	typedef int		(*match_fn)(int c);

private:
	match_fn 	_fn;
	bool 		_empty;
	int 		_min;
	int 		_max;

public:
	TakeWhile(match_fn fn, bool empty = false);
	TakeWhile(match_fn fn, int min, int max);

	result_type operator()(const slice &input);
};

#endif //WEBSERV_TAKEWHILE_HPP
