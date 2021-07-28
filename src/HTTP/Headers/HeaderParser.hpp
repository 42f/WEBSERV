//
// Created by alena on 28/07/2021.
//

#ifndef WEBSERV_HEADERPARSER_HPP
#define WEBSERV_HEADERPARSER_HPP

#include "Header.hpp"

template<typename Name = FieldName, typename P = FieldValue>
class HeaderParser: public Parser<Header>
{
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
HeaderParser<Tag, Value>	header(std::string name, Value parser)
{
	return HeaderParser<Tag, Value>(name, parser);
}

#endif //WEBSERV_HEADERPARSER_HPP
