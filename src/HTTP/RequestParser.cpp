//
// Created by alena on 30/07/2021.
//

#include "RequestParser.hpp"

/*
 * RequestParser
 */
RequestParser::RequestParser() { }

RequestParser::result_type	RequestParser::operator()(const slice& input)
{
	ParserResult<tuple<methods::s_method, Target, Version> > line = RequestLine()(input);
	Request		req;

	if (line.is_ok())
	{
		tuple<methods::s_method, Target, Version>	info = line.unwrap();
		if (info.first == methods::OTHER)
			return RequestParser::result_type::err(line.left(), error("Not implemented", status::NotImplemented));
		if (info.third != Version(1, 1))
			return RequestParser::result_type::err(line.left(), error("Unsupported version", status::UnsupportedVersion));
		req = Request(info.first, info.second, info.third);
	}
	else {
		return result_type::err(line.left(), error("Failed to parse request line"));
	}
	ParserResult<std::vector<Header> >	res =
			terminated(many(terminated(Headers(), newline)), Newline())(line.left());
	std::cout << "****Parsing****" << std::endl << req << std::endl;
	if (res.is_ok())
	{
		std::vector<Header>	&v = res.unwrap();
		for (std::vector<Header>::iterator it = v.begin(); it != v.end(); it++)
		{
			req.set_header(*it);
			std::cout << *it << std::endl;
		}
		return result_type::ok(res.left(), req);
	}
	std::cerr << res.unwrap_err() << std::endl;
	return res.convert<Request>();
	// TODO if this is not here, either the request is incomplete or ill-formatted
}
