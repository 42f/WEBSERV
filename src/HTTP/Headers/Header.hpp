//
// Created by alena on 24/06/2021.
//

#ifndef WEBSERV_HEADER_HPP
#define WEBSERV_HEADER_HPP

#include "export.hpp"
#include "Tokens.hpp"

/*
 * FieldName = token
 */
typedef Token FieldName;

/*
 * FieldValue  = *(field-vchar [ 1*( SP / HTAB ) field-vchar ])
 */
class FieldValue : public Parser<slice>
{
public:
	FieldValue();
	result_type	operator()(const slice &input);
};

/* ************************************************************************** */

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

	std::string	value();
	std::string	value() const;

	std::string	name();
	std::string	name() const;

	friend std::ostream &operator<<(std::ostream& stream, const Header& header);
};

/* ************************************************************************** */


#endif //WEBSERV_HEADERS_HPP
