//
// Created by alena on 28/07/2021.
//

#include "ConfigParser.hpp"

ConfigParser::ConfigParser() { }

ConfigParser::result_type	ConfigParser::operator()(const slice &input)
{
	ParserResult<std::vector<config::Server> >	lst = terminated(many(alt(
			map(Comment(), config::Server::invalid),
			map(newline, config::Server::invalid),
			wrap_error("Server block", fail(ServerBlock())))
	), Eof())(input);

	if (lst.is_err())
		return lst.unwind(input, "Config parsing");
	std::vector<config::Server>		in = lst.unwrap();
	std::vector<config::Server>		out;
	for (std::vector<config::Server>::iterator it = in.begin(); it != in.end(); it++)
		if (!it->is_invalid())
			out.push_back(*it);
	return lst.map(out);
}

/* ************************************************************************** */
