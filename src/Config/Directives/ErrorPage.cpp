//
// Created by alena on 14/07/2021.
//

#include "ErrorPage.hpp"
/*
 * ErrorPage = error_page RWS isdigit [ isdigit ] RWS path
 */

ErrorPage::ErrorPage() { }

ErrorPage::result_type ErrorPage::operator()(const slice &input)
{
	ParserResult<tuple<slice, slice, std::vector<slice>, slice > > res = sequence(as_slice(
			sequence(Tag("error_page"), rws)), TakeWhile(std::isdigit), many(
			preceded(rws, TakeWhile(std::isdigit)), true),
																			preceded(rws, TakeUntil(";")))(input);
	if (res.is_ok())
	{
		slice	path = res.unwrap().fourth;
		std::vector<slice>	codes = res.unwrap().third;
		codes.insert(codes.begin(), res.unwrap().second);
		std::vector<std::pair<int, std::string > > map;
		map.reserve(codes.size());
		for (std::vector<slice>::iterator it = codes.begin(); it != codes.end(); it++)
			map.push_back(std::make_pair(it->to_int(), path.to_string()));
		return result_type::ok(res.left(), map);
	}
	return res.convert<data_type>().unwind(input, "ErrorPage: failed to parse");
}