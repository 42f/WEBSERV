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
	return take_with(sequence(Char('/'), Segment()))(input);
}

/*
 * Query = *( pchar / "/" / "?" )
 */
Query::Query() { }
Query::result_type	Query::operator()(const slice &input)
{
	return take_with(alt(Pchar(), OneOf("/?")), true)(input);
}

/*
 * OriginForm = AbsolutePath [ "?" Query]
 */
Target	as_target(const slice& input, ParserResult<tuple<slice, slice> >&res)
{
	(void)input;
	return Target::from(res.unwrap().first, res.unwrap().second);
}

OriginForm::result_type OriginForm::operator()(const slice &input)
{
	return sequence(AbsolutePath(), opt(preceded(Char('?'), Query())))(input)
			.map(input, as_target);
}
