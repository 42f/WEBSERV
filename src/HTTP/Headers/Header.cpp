//
// Created by alena on 28/07/2021.
//

#include "Header.hpp"

/*
 * FieldValue  = *(field-vchar [ 1*( SP / HTAB ) field-vchar ])
 */
FieldValue::FieldValue() { }
FieldValue::result_type	FieldValue::operator()(const slice &input)
{
	static const Alt<Match, HexChar> charset = alt(vchar, obs);
	static const TakeWith<OneOf>	spaces = take_with(OneOf(" \t"));

	ParserResult<slice> res = take_with(sequence(charset, opt(sequence(spaces, charset))))(input);
	return res;
}

/*
 * Header
 */
Header::Header(slice name, slice value): _name(name), _value(value) { }

std::string	Header::value() { return this->_value.to_string(); }
std::string	Header::value() const { return this->_value.to_string(); }

std::string	Header::name() { return this->_name.to_string(); }
std::string	Header::name() const { return this->_name.to_string(); }

std::ostream &operator<<(std::ostream& stream, const Header& header)
{
	stream << header._name.to_string() << ": " << header._value.to_string();
	return stream;
}