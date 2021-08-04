//
// Created by alena on 07/07/2021.
//

#ifndef WEBSERV_BLOC_HPP
#define WEBSERV_BLOC_HPP

#include "parser/export.hpp"

namespace config
{
	template<typename Header, typename Content>
	class Bloc: public Parser<tuple<typename Header::data_type, typename Content::data_type> >
	{
	private:
		Header	_header;
		Content	_content;

	public:
		typedef tuple<typename Header::data_type, typename Content::data_type>	data_type;
		typedef ParserResult<data_type>	result_type;
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
		Bloc(Header header, Content content): _header(header), _content(content) { }
/*
** --------------------------------- METHODS ----------------------------------
*/
		result_type operator()(const slice &input)
		{
			return preceded(many(newline, true), sequence(
					delimited(ows, _header, rws),
					delimited(sequence(Char('{'), newline), _content, sequence(many(newline, true), ows, Char('}'), newline)))
			)(input);
		}
	};
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/
template<typename Header, typename Content>
config::Bloc<Header,Content>	bloc(Header head, Content content) { return config::Bloc<Header, Content>(head, content); }

/* ************************************************************************** */

#endif //WEBSERV_BLOC_HPP
