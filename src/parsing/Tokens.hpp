//
// Created by alena on 14/06/2021.
//

#ifndef WEBSERV_TOKENS_HPP
#define WEBSERV_TOKENS_HPP

#include "Parser.hpp"
#include "parser/combinators.hpp"

/*
 * OWS = *(' ' | '\t')
 */
class OWS: public Parser<slice>
{
public:
	OWS();

	result_type		operator()(const slice &input);
};

/*
 * RWS = 1*(' ' | '\t')
 */
class RWS: public Parser<slice>
{
public:
	RWS();

	result_type		operator()(const slice &input);
};

/*
 * Newline = '\r\n' or '\n'
 */
class Newline: public Parser<slice>
{
private:
	Tag		_both;
	Tag		_n;
public:
	Newline();
	result_type 	operator()(const slice& input);
};

const Newline newline = Newline();

/*
 * Token = 1 * tchar = "!" | "#" | "$" | "%" | "&" | "'" | "*"| "+" | "-" | "." | "^" | "_" | "`" | "|" | "~" | DIGIT | ALPHA
 */
class Token: public Parser<slice>
{
public:
	Token();
	result_type		operator()(const slice &input);
};

/*
 * quoted-string  = DQUOTE *( qdtext / quoted-pair ) DQUOTE
 */
class QuotedText: public Parser<slice>
{
public:
	QuotedText();
	result_type	operator()(const slice &input);
};

const Char	single_space = Char(' ');
const RWS rws = RWS();
const OWS ows = OWS();
const Match		vchar = Match(isprint);
const HexChar obs = HexChar('\x80', '\xFF');
const Token token = Token();
const QuotedText quoted_text = QuotedText();

#endif //WEBSERV_TOKENS_HPP
