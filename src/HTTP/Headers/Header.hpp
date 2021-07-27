//
// Created by alena on 24/06/2021.
//

#ifndef WEBSERV_HEADER_HPP
#define WEBSERV_HEADER_HPP

#include "slice.hpp"
#include "Error.hpp"

/*
 * FieldName = token = 1 * tchar = "!" | "#" | "$" | "%" | "&" | "'" | "*"| "+" | "-" | "." | "^" | "_" | "`" | "|" | "~" | DIGIT | ALPHA
 */

static const OneOf charset = OneOf("!#$%&'*+-.^_`|~");

class FieldName : public Parser<slice>
{
public:
	FieldName() { }
	result_type operator()(const slice &input)
	{
		return take_with(alt(charset, digit, alpha))(input);
	}
};

/*
 * FieldValue  = *(field-vchar [ 1*( SP / HTAB ) field-vchar ])
 */

class FieldValue : public Parser<slice>
{
public:
	FieldValue() { }
	result_type operator()(const slice &input)
	{
		static const Alt<Match, HexChar> vchar = alt(Match(isprint), HexChar('\x80', '\xFF'));
		ParserResult<slice> res = take_with(sequence(vchar, opt(sequence(take_with(OneOf(" \t")), vchar))))(input);
		return res;
	}
};

class Header
{
private:
	slice		_name;
	slice		_value;

public:
	Header(slice name, slice value): _name(name), _value(value) { }

	static Header	from_tuple(tuple<slice, slice> input) {
		return Header(input.first, input.second);
	}

	std::string	value() { return this->_value.to_string(); }
	std::string	value() const { return this->_value.to_string(); }

	std::string	name() { return this->_name.to_string(); }
	std::string	name() const { return this->_name.to_string(); }

	friend std::ostream &operator<<(std::ostream& stream, const Header& header)
	{
		stream << header._name.to_string() << " : " << header._value.to_string();
		return stream;
	}
};

template<typename Name = FieldName, typename P = FieldValue>
class HeaderParser: public Parser<Header> {
private:
	P				_field;
	Name			_name;

public:
	HeaderParser(): _field(FieldValue()), _name(FieldName()) { }
	HeaderParser(std::string name, P parser): _field(parser), _name(Tag(name)) { }

	result_type		operator()(const slice &input) {
		return map(separated(
			terminated(_name, Char(':')), ows,
			terminated(as_slice(_field), ows)),
			Header::from_tuple)(input);
	}
};

template<typename Value>
HeaderParser<Tag, Value>	header(std::string name, Value parser) {
	return HeaderParser<Tag, Value>(name, parser);
}

//TODO cpp

#endif //WEBSERV_HEADERS_HPP
