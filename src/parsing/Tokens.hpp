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
 * SingleSpace = ' '
 */
class SingleSpace: public Parser<char>
{
public:
	SingleSpace();
	result_type		operator()(const slice &input);
};

const SingleSpace single_space = SingleSpace();
const RWS rws = RWS();
const OWS ows = OWS();

#endif //WEBSERV_TOKENS_HPP
