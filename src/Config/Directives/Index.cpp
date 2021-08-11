//
// Created by alena on 14/07/2021.
//

#include "Index.hpp"
/*
 * Index = index index.html;
 */

Index::Index() { }

Index::result_type Index::operator()(const slice &input)
{
	return preceded(sequence(Tag("index"), rws),TakeUntil(";"))(input);
}

/* ************************************************************************** */

/*
 * AutoIndex = autoindex on | off
 */

AutoIndex::AutoIndex() { }

AutoIndex::result_type AutoIndex::operator()(const slice &input)
{
	ParserResult<slice> res = preceded(sequence(Tag("autoindex"), rws), alt(Tag("on"), Tag("off")))(input);
	if (res.is_ok())
		return res.map(res.unwrap().ieq("on"));
	return res.convert<bool>();
}

/* ************************************************************************** */
