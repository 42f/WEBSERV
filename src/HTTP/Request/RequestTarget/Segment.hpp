//
// Created by alena on 18/06/2021.
//

#ifndef WEBSERV_SEGMENT_HPP
#define WEBSERV_SEGMENT_HPP

#include "export.hpp"

const std::string HEXBASE = "0123456789ABCDEF";
const OneOf		HEXDIG = OneOf("0123456789ABCDEFabcdef");

/*
 * PctEncoded = "%" HEXDIG HEXDIG
 */
class PctEncoded : public Parser<char>
{
public:

	PctEncoded();

	result_type	operator()(const slice &input);
};

/*
 * unreserved  = ALPHA | DIGIT | "-" | "." | "_" | "~"
 */
class Unreserved: public Parser<char>
{
public:

	Unreserved();

	result_type	operator()(const slice &input);
};

/*
 * Pchar = unreserved | pct-encoded | sub-delims | ":" | "@"
 */
const OneOf SubDelims = OneOf("!$&'()*+,;=");

class Pchar : public Parser<char>
{
public:
	Pchar();

	result_type	operator()(const slice &input);
};

/*
 * Segment = *Pchar
 * NonzeroSegment = 1*Pchar (Segment(false))
 */
class Segment : public Parser<slice>
{
private:
	bool	_empty;

public :
	Segment(bool empty = true);

	result_type	operator()(const slice &input);
};

#endif //WEBSERV_SEGMENT_HPP
