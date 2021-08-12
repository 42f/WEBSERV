//
// Created by alena on 28/07/2021.
//

#include "ConfigParser.hpp"

ConfigParser::ConfigParser() { }

ConfigParser::result_type	ConfigParser::operator()(const slice &input)
{
	return terminated(many(
		delimited(
			take_with(alt(Comment(), newline), true),
			wrap_error("Server block", fail(ServerBlock())),
			take_with(alt(Comment(), newline), true))),
		Eof())(input);
}

/* ************************************************************************** */
