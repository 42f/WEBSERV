//
// Created by alena on 24/06/2021.
//

#ifndef WEBSERV_HEXCHAR_HPP
#define WEBSERV_HEXCHAR_HPP

#include "Parser.hpp"

/*
 * Matches a single character in the given hex range
 * Eg: HexChar("H") ("HelloWorld")
 *  => result: Ok("H", left : elloWorld)
 */
class HexChar: public Parser<char>
{
private:
	int		_l;
	int		_h;

public:
	HexChar(int low, int high);

	result_type		operator()(const slice& input);
};

namespace streaming {
	class HexChar: public Parser<char>
	{
	private:
		int		_l;
		int		_h;

	public:
		HexChar(int low, int high);

		result_type		operator()(const slice& input);
	};
}

#endif //WEBSERV_HEXCHAR_HPP
