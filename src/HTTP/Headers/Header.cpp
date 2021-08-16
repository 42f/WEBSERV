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
	ParserResult<slice> res = take_until_match(newline)(input);
	if (res.is_err())
		return res.unwind(input, "Field value");
	slice	s = res.unwrap();
	size_t	o = s.size - 1;
	while (o && std::isspace(s.p[o]))
	{
		o--;
		s.size--;
	}
	return result_type::ok(res.left(), s);
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
