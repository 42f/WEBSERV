//
// Created by alena on 28/07/2021.
//

#include "ServerBlock.hpp"

/*
 * ServerContent is a line for block server
 * listen | server_name ...
 */
ServerContent::ServerContent() { }

ServerContent::result_type ServerContent::operator()(const slice &input)
{
	config::Server cfg;
	config::Server::use(&cfg);

	ParserResult<slice> res = as_slice(many(alt(
			as_slice(map(Location(), config::Server::with_location)),
			as_slice(map(directive(Listen()), config::Server::with_addr)),
			as_slice(map(directive(ServerName()), config::Server::name)),
			as_slice(map(directive(Root()), config::Server::root)),
			as_slice(map(directive(Cgi()), config::Server::cgi)),
			as_slice(map(directive(ClientMaxBodySize()), config::Server::max_size)),
			as_slice(map(directive(Index()), config::Server::index)),
			alt(
					as_slice(map(directive(ErrorPage()), config::Server::error_codes)),
					as_slice(map(comment, config::Server::dump)),
					as_slice(map(empty_line, config::Server::dump))
			)
	)))(input);
	return res.map(cfg);
}

/*
 * A create the server block
 */
ServerBlock::ServerBlock() { }

ServerBlock::result_type	ServerBlock::operator()(const slice &input)
{
	ParserResult<tuple<slice, config::Server> >	res = bloc(Tag("server"), ServerContent())(input);
	if (res.is_err())
		return res.convert<config::Server>();
	return res.map(res.unwrap().second);
}