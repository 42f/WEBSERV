//
// Created by alena on 15/07/2021.
//

#include "RequestLine.hpp"

/*
 * RequestMethod = [GET | POST | DELETE] SP
 */
RequestMethod::RequestMethod() { }

RequestMethod::result_type RequestMethod::operator()(const slice &input)
{
	return alt(
			map(Tag("GET"), methods::as_get),
			map(Tag("POST"), methods::as_post),
			map(Tag("DELETE"), methods::as_delete),
			map(TakeWhile(std::isalpha), methods::as_other))(input);
}

std::ostream &operator<<(std::ostream& st, const methods::s_method& method)
{
	switch (method) {
		case methods::GET:
			st << "GET";
			break;
		case methods::POST:
			st << "POST";
			break;
		case methods::DELETE:
			st << "DELETE";
			break;
		default:
			break;
	}
	return st;
}

/* ************************************************************************** */


/*
 * RequestTarget = OriginForm (| absolute-form)
 */
RequestTarget::RequestTarget() { }

RequestTarget::result_type	RequestTarget::operator()(const slice &input)
{
	return OriginForm()(input);
}

/* ************************************************************************** */


/*
 * RequestVersion = HTTP-version = HTTP "/" DIGIT "." DIGIT
 */
RequestVersion::RequestVersion() { }

RequestVersion::result_type	RequestVersion::operator()(const slice &input)
{
	return map(preceded(
		Tag("HTTP/"),
		separated(digit, Char('.'), digit)),
			Version::from_tuple)(input);
}

/* ************************************************************************** */

/*
 * RequestLine = RequestMethod SP RequestTarget SP RequestVersion CRLF
 */
RequestLine::RequestLine() { }

RequestLine::result_type	RequestLine::operator()(const slice &input)
{
	return terminated(sequence(
			terminated(RequestMethod(), single_space),
			terminated(RequestTarget(), single_space),
			RequestVersion()),newline)(input);
}

/* ************************************************************************** */

