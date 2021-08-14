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

	for (size_t i = 0; i < len && i < input.size; i++)
		if (std::tolower(input.p[i]) != std::tolower(_tag[i]))
			return result_type::err(input, error("ITag: no match for |" + _tag + "|"));
	if (input.size < len)
		return result_type::fail(input, error("incomplete", status::Incomplete));
	return result_type::ok( input.from(len), input.take(len));
}
