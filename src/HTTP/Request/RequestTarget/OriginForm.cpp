//
// Created by alena on 24/06/2021.
//

#include "OriginForm.hpp"

/*
 * AbsolutePath = 1*( "/" segment )
 */
AbsolutePath::AbsolutePath() { }

AbsolutePath::result_type	AbsolutePath::operator()(const slice &input)
{
	ParserResult<std::vector<slice> >	res = many(preceded(streaming::Char('/'),
		Segment()))(input);
	if (res.is_err())
		return res.convert<std::string>();
	std::vector<slice>	v = res.unwrap();
	std::vector<slice>	out;
	for (std::vector<slice>::iterator it = v.begin(); it != v.end(); it++) {
		if (*it == ".")
			continue;
		else if (*it == "..") {
			if (out.empty())
				return result_type::fail(input, error("Invalid path", status::BadRequest));
			else
				out.pop_back();
		} else
			out.push_back(*it);
	}
	std::string r = "/";
	for (std::vector<slice>::iterator it = out.begin(); it != out.end(); it++) {
		r += it->to_string();
		if (it + 1 != out.end())
			r += "/";
	}
	return result_type::ok(res.left(), r);
}

/* ************************************************************************** */

/*
 * Query = *( pchar / "/" / "?" )
 */
Query::Query() { }

Query::result_type	Query::operator()(const slice &input)
{
	return take_with(alt(Pchar(), OneOf("/?")), true)(input);
}

/* ************************************************************************** */

/*
 * OriginForm = AbsolutePath [ "?" Query]
 */
Target	as_target(const slice& input, ParserResult<tuple<std::string, slice> >&res)
{
	(void)input;
	return Target::from(res.unwrap().first, res.unwrap().second);
}

OriginForm::OriginForm() { };

OriginForm::result_type OriginForm::operator()(const slice &input)
{
	return sequence(AbsolutePath(), opt(preceded(Char('?'), Query())))(input)
			.map(input, as_target);
}

/* ************************************************************************** */
