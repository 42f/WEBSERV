//
// Created by alena on 30/07/2021.
//

#include "RequestParser.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
RequestParser::RequestParser() { }

/*
** --------------------------------- METHODS ----------------------------------
*/
RequestParser::result_type	RequestParser::operator()(const slice& input)
{
	ParserResult<tuple<methods::s_method, Target, Version> > line = RequestLine()(input);
	Request		req;

	if (line.is_ok())
	{
		tuple<methods::s_method, Target, Version>	info = line.unwrap();
		if (info.first == methods::OTHER)
			return RequestParser::result_type::err(line.left(), error("Not implemented", status::NotImplemented));
		if (info.third != Version('1', '1'))
			return RequestParser::result_type::err(line.left(), error("Unsupported version", status::HTTPVersionNotSupported));
		req = Request(info.first, info.second, info.third);
	}
	else
		return line.convert<Request>().unwind(input, "Failed to parse request line");
	ParserResult<std::vector<Header> >	res =
			terminated(many(terminated(Headers(), newline)), Newline())(line.left());
	if (res.is_ok())
	{
		std::vector<Header>	&v = res.unwrap();
		for (std::vector<Header>::iterator it = v.begin(); it != v.end(); it++)
			req.set_header(*it);
		return result_type::ok(res.left(), req);
	}
	return res.convert<Request>();
}
/* ************************************************************************** */

