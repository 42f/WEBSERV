//
// Created by alena on 28/07/2021.
//

#include "ConfigParser.hpp"

ConfigParser::ConfigParser() { }

ConfigParser::result_type	ConfigParser::operator()(const slice &input)
{
	return terminated(many(
			delimited(many(newline, true),
					  fail(ServerBlock()),
					  many(newline, true))),
					  sequence(take_with(Comment(), true), take_with(Newline(), true), Eof()))(input);
}

/* ************************************************************************** */
