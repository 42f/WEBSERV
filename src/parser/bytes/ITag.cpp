//
// Created by alena on 14/07/2021.
//

#include "Itag.hpp"

/*
 * Matches a string without case
 * Eg: ITag("HELLO") ("HelloWorld")
 *  => result: Ok("Hello", left : World)
 */
ITag::ITag(const std::string &tag) : _tag(tag) { }

ITag::result_type	ITag::operator()(const slice &input)
{
	size_t 	len = _tag.length();
	if (input.icontains(_tag) == 0)
		return result_type::ok( input.from(len), input.take(len));
	return result_type::err(input, error("ITag: no match for |" + _tag + "|"));
}