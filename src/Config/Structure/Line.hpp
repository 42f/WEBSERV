//
// Created by alena on 07/07/2021.
//

#ifndef WEBSERV_LINE_HPP
#define WEBSERV_LINE_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

class EmptyLine: public Parser<slice>
{
public:
	EmptyLine();

	result_type operator()(const slice &input);
};

const EmptyLine empty_line = EmptyLine();

/* ************************************************************************** */

class Comment: public Parser<slice>
{
public:
	Comment();

	result_type operator()(const slice &input);

};

const Comment comment = Comment();

/* ************************************************************************** */

/*
 * Wraps directives with optionnal spaces at the start and a ; followed by a newline at the end
 * Directive = OWS P; OWS CRLF
 */

template<typename P>
class Directive: public Parser<typename P::data_type>
{
private:
	P	_p;
public:
	Directive(P parser): _p(parser) { }

	ParserResult<typename P::data_type>	operator()(const slice &input)
	{
		return preceded(many(newline, true), delimited(ows, _p, sequence(Tag(";"), ows, many(empty_line))))(input);
	}
};

template<typename P>
Directive<P>	directive(P parser) { return Directive<P>(parser); }

/* ************************************************************************** */

#endif //WEBSERV_LINE_HPP
