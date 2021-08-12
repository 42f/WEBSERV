//
// Created by alena on 14/07/2021.
//

#include "Tokens.hpp"

/*
 * OWS = *(' ' | '\t')
 */
OWS::OWS() { }

OWS::result_type	OWS::operator()(const slice &input)
{
	return take_with(alt(Char(' '), Char('\t')), true)(input);
}

/*
 * RWS = 1*(' ' | '\t')
 */
RWS::RWS() { }

RWS::result_type	RWS::operator()(const slice &input)
{
	return take_with(alt(Char(' '), Char('\t')))(input);
}

/*
 * Newline = '\r\n' or '\n'
 */
Newline::Newline(): _both(Tag("\r\n")), _n(Tag("\n")) { }

Newline::result_type	Newline::operator()(const slice& input)
{
	result_type		res = _both(input);
	if (res.is_err())
		return _n(input);
	return (res);
}

/*
 * Token = 1 * tchar = "!" | "#" | "$" | "%" | "&" | "'" | "*"| "+" | "-" | "." | "^" | "_" | "`" | "|" | "~" | DIGIT | ALPHA
 */
static const streaming::OneOf charset = streaming::OneOf("!#$%&'*+-.^_`|~");

Token::Token() { }

Token::result_type	Token::operator()(const slice &input)
{
	return take_with(alt(charset, streaming::digit, streaming::alpha))(input);
}

/*
 * quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
 * qdtext = HTAB / SP /%x21 / %x23-5B / %x5D-7E / obs-text
 * obs-text = %x80-FF
 * quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
 */
QuotedText::QuotedText() { }

static const streaming::OneOf QD_TEXT_CHARSET = streaming::OneOf("\t \x21");

QuotedText::result_type QuotedText::operator()(const slice &input)
{
	static const streaming::Char DQUOTE = streaming::Char('\"');
	static const Alt<streaming::OneOf, streaming::HexChar, streaming::HexChar, streaming::HexChar>
		QD_TEXT = alt(
				QD_TEXT_CHARSET,
				streaming::HexChar(0x23, 0x5B),
				streaming::HexChar(0x5D, 0x7E),
			obs);
	static const Preceded<streaming::Char, Alt<streaming::OneOf, streaming::Match, streaming::HexChar> >
		QD_PAIR = preceded(
				streaming::Char('\\'),
				alt(QD_TEXT_CHARSET, vchar, obs));

	return as_slice(delimited(DQUOTE, take_with(alt(QD_TEXT, QD_PAIR)), DQUOTE))(input);
}

/* ************************************************************************** */

namespace streaming {
	OWS::OWS() { }

	OWS::result_type	OWS::operator()(const slice &input)
	{
		return take_with(alt(
				streaming::Char(' '),
				streaming::Char('\t')),
						 true)(input);
	}

	RWS::RWS() { }

	RWS::result_type	RWS::operator()(const slice &input)
	{
		return take_with(alt(streaming::Char(' '), streaming::Char('\t')))(input);
	}

	Newline::Newline(): _both(streaming::Tag("\r\n")), _n(streaming::Tag("\n")) { }

	Newline::result_type	Newline::operator()(const slice& input)
	{
		result_type		res = _both(input);
		if (res.is_err())
			return _n(input);
		return (res);
	}

}
