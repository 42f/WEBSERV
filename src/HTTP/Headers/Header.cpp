//
// Created by alena on 28/07/2021.
//

#include "Header.hpp"

/*
 * FieldName = token = 1 * tchar = "!" | "#" | "$" | "%" | "&" | "'" | "*"| "+" | "-" | "." | "^" | "_" | "`" | "|" | "~" | DIGIT | ALPHA
 */
FieldName::FieldName() { }

FieldName::result_type	FieldName::operator()(const slice &input)
{
	return take_with(alt(charset, digit, alpha))(input);
}

/*
 * FieldValue  = *(field-vchar [ 1*( SP / HTAB ) field-vchar ])
 */
FieldValue::FieldValue() { }
FieldValue::result_type	FieldValue::operator()(const slice &input)
{
	static const Alt<Match, HexChar> vchar = alt(Match(isprint), HexChar('\x80', '\xFF'));
	ParserResult<slice> res = take_with(sequence(vchar, opt(sequence(take_with(OneOf(" \t")), vchar))))(input);
	return res;
}

/*
 * Header
 */
Header::Header(slice name, slice value): _name(name), _value(value) { }

Header	Header::from_tuple(tuple<slice, slice> input) {
	return Header(input.first, input.second);
}

std::string	Header::value() { return this->_value.to_string(); }
std::string	Header::value() const { return this->_value.to_string(); }

std::string	Header::name() { return this->_name.to_string(); }
std::string	Header::name() const { return this->_name.to_string(); }

std::ostream &operator<<(std::ostream& stream, const Header& header)
{
	stream << header._name.to_string() << " : " << header._value.to_string();
	return stream;
}