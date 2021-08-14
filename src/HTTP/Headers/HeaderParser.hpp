//
// Created by alena on 28/07/2021.
//

#ifndef WEBSERV_HEADERPARSER_HPP
#define WEBSERV_HEADERPARSER_HPP

#include "Header.hpp"

/*
 * HeaderParser = field-name ":" OWS field-value OWS
 */
template<typename Name = FieldName, typename P = FieldValue>
class HeaderParser: public Parser<Header>
{
private:
	P				_field;
	Name			_name;

public:
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
	HeaderParser(): _field(FieldValue()), _name(FieldName()) { }
	HeaderParser(std::string name, P parser): _field(parser), _name(Tag(name)) { }
/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/
	result_type		operator()(const slice &input)
	{
		typename Name::result_type name = terminated(_name, sequence(Char(':'), ows))(input);
		if (name.is_err())
			return name.template convert<Header>();
		slice	left = name.left();
		typename P::result_type	value = terminated(as_slice(_field), ows)(left);
		if (value.is_err())
			return value.template convert<Header>();
		return result_type::ok(value.left(), Header(name.unwrap(), value.unwrap()));
	}
};
/*
** --------------------------------- ACCESSOR ---------------------------------
*/
template<typename Value>
HeaderParser<Tag, Value>	header(std::string name, Value parser)
{
	return HeaderParser<Tag, Value>(name, parser);
}

/* ************************************************************************** */



#endif //WEBSERV_HEADERPARSER_HPP
