//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_TAG_HPP
#define WEBSERV_TAG_HPP

#include "Parser.hpp"

/*
 * Matches a string exactly
 * Eg: Tag("Hello") ("HelloWorld")
 *  => result: Ok("Hello", left : World)
 */
class Tag: public Parser<slice>
{
private:
	std::string		_tag;

public:
	Tag(const std::string &tag);

	result_type operator()(const slice &input);
};

namespace streaming {
	class Tag: public Parser<slice>
	{
	private:
		std::string		_tag;

	public:
		Tag(const std::string &tag);

		result_type operator()(const slice &input);
	};
}


#endif //WEBSERV_TAG_HPP