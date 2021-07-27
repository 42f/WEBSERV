//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_ITAG_HPP
#define WEBSERV_ITAG_HPP

#include "Parser.hpp"

/*
 * Matches a string without case
 * Eg: ITag("HELLO") ("HelloWorld")
 *  => result: Ok("Hello", left : World)
 */
class ITag: public Parser<slice>
{
private:
	std::string		_tag;

public:
	ITag(const std::string &tag);

	result_type operator()(const slice &input);
};

#endif //WEBSERV_ITAG_HPP
