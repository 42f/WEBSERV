//
// Created by alena on 28/07/2021.
//

#include "Header.hpp"

/*
 * FieldValue  = *(field-vchar [ 1*( SP / HTAB ) field-vchar ])
 */
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
FieldValue::FieldValue() { }

/*
** --------------------------------- METHODS ----------------------------------
*/
FieldValue::result_type	FieldValue::operator()(const slice &input)
{
	static const Alt<Match, HexChar> charset = alt(vchar, obs);
	static const TakeWith<OneOf>	spaces = take_with(OneOf(" \t"));

	ParserResult<slice> res = take_with(sequence(charset, opt(sequence(spaces, charset))))(input);
	return res;
}

/* ************************************************************************** */

/*
 * Header
 */
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Header::Header(slice name, slice value): _name(name.to_string()), _value(value.to_string()) { }

/*
** --------------------------------- METHODS ----------------------------------
*/
const std::string	&Header::value() { return this->_value; }
const std::string	&Header::value() const { return this->_value; }

const std::string	&Header::name() { return this->_name; }
const std::string	&Header::name() const { return this->_name; }

/*
** --------------------------------- OVERLOAD ---------------------------------
*/
std::ostream &operator<<(std::ostream& stream, const Header& header)
{
	stream << header._name << ": " << header._value;
	return stream;
}

/* ************************************************************************** */
