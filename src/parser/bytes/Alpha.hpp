//
// Created by alena on 18/06/2021.
//

#ifndef WEBSERV_ALPHA_HPP
#define WEBSERV_ALPHA_HPP

#include "Parser.hpp"

/*
 * Matches a single alphabetic character
 * Eg: Alpha("A") ("AbC")
 *  => result: Ok("A", left : bc)
 */

class Alpha: public Parser<char>
{
public:
	typedef Parser::result_type		result_type;

	Alpha();

	result_type		operator()(const slice& input);
};

const Alpha	alpha = Alpha();

namespace streaming {

	class Alpha: public Parser<char>
	{
	public:
		typedef Parser::result_type		result_type;

		Alpha();

		result_type		operator()(const slice& input);
	};

	const Alpha	alpha = Alpha();
}

#endif //WEBSERV_ALPHA_HPP
