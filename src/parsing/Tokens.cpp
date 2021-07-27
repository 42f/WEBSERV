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
 * SingleSpace = ' '
 */
SingleSpace::SingleSpace() { }

SingleSpace::result_type	SingleSpace::operator()(const slice &input)
{
	return Char(' ')(input);
}