//
// Created by alena on 24/06/2021.
//

#ifndef WEBSERV_HEADER_HPP
#define WEBSERV_HEADER_HPP

#include "export.hpp"
#include "Tokens.hpp"

/*
 * FieldName = token = 1 * tchar = "!" | "#" | "$" | "%" | "&" | "'" | "*"| "+" | "-" | "." | "^" | "_" | "`" | "|" | "~" | DIGIT | ALPHA
 */
static const OneOf charset = OneOf("!#$%&'*+-.^_`|~");

class FieldName : public Parser<slice>
{
public:
	FieldName();
	result_type operator()(const slice &input);
};

/*
 * FieldValue  = *(field-vchar [ 1*( SP / HTAB ) field-vchar ])
 */
class FieldValue : public Parser<slice>
{
public:
	FieldValue();
	result_type operator()(const slice &input);
};

/*
 * Header
 */
class Header
{
private:
	slice		_name;
	slice		_value;

public:
	Header(slice name, slice value);

	static Header	from_tuple(tuple<slice, slice> input);

	std::string	value();
	std::string	value() const;

	std::string	name();
	std::string	name() const;

	friend std::ostream &operator<<(std::ostream& stream, const Header& header);
};


#endif //WEBSERV_HEADERS_HPP
