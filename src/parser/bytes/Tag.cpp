//
// Created by alena on 14/07/2021.
//

#include "Tag.hpp"

/*
 * Matches a string exactly
 * Eg: Tag("Hello") ("HelloWorld")
 *  => result: Ok("Hello", left : World)
 */
Tag::Tag(const std::string &tag) : _tag(tag) { }

Tag::result_type	Tag::operator()(const slice &input)
{
	size_t len = _tag.length();
	if (input.contains(_tag) == 0) {
		return result_type::ok(input.from(len), input.take(len));
	}
	return result_type::err(input, error("Tag: no match for |" + _tag + "|"));
}

namespace streaming {

	Tag::Tag(const std::string &tag): _tag(tag) { }

	Tag::result_type Tag::operator()(const slice &input) {
		size_t len = _tag.length();

		for (size_t i = 0; i < len && i < input.size; i++)
			if (input.p[i] != _tag[i])
				return result_type::err(input, error("Tag: no match for |" + _tag + "|"));
		if (input.size < len)
			return result_type::fail(input, error("incomplete", status::Incomplete));
		return result_type::ok(input.from(len), input.take(len));
	}
}