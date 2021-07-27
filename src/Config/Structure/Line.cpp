//
// Created by alena on 14/07/2021.
//

#include "Line.hpp"

/*
 * EmptyLine
 */
EmptyLine::EmptyLine() { }

EmptyLine::result_type EmptyLine::operator()(const slice &input)
{
	return as_slice(preceded(ows, newline))(input);
}

/*
 * Comment
 */
Comment::Comment() { }

Comment::result_type Comment::operator()(const slice &input)
{
	return preceded(many(newline, true), delimited(sequence(ows, Tag("#")), take_until_match(newline, true), empty_line))(input);
}
