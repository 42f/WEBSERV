//
// Created by alena on 15/07/2021.
//

#include "Charset.hpp"

/*
 * PctEncoded = "%" HEXDIG HEXDIG
 */
PctEncoded::PctEncoded() { }

PctEncoded::result_type	PctEncoded::operator()(const slice &input)
{
	ParserResult<tuple<char, char> >	res = preceded(Char('%'), sequence(HEXDIG, HEXDIG))(input);
	if (res.is_err())
		return res.convert<char>();
	tuple<char, char>	data = res.unwrap();
	char c = (HEXBASE.find(std::toupper(data.first)) << 4) |
			 HEXBASE.find(std::toupper(data.second));
	return result_type::ok(res.left(), c);
}

/* ************************************************************************** */


/*
 * unreserved  = ALPHA | DIGIT | "-" | "." | "_" | "~"
 */
Unreserved::Unreserved() { }

Unreserved::result_type	Unreserved::operator()(const slice &input)
{
	return alt(alpha, digit, OneOf("-._~"))(input);
}

/* ************************************************************************** */


/*
 * Pchar = unreserved | pct-encoded | sub-delims | ":" | "@"
 */
Pchar::Pchar() { }

Pchar::result_type	Pchar::operator()(const slice &input)
{
	return alt(Unreserved(), PctEncoded(), SubDelims, OneOf(":@"))(input);
}

/* ************************************************************************** */


/*
 * Segment = *Pchar
 * NonzeroSegment = 1*Pchar (Segment(false))
 */
Segment::Segment(bool empty) : _empty(empty) { }

Segment::result_type	Segment::operator()(const slice &input)
{
	return take_with(Pchar(), _empty)(input);
}

/* ************************************************************************** */
