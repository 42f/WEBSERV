//
// Created by alena on 14/07/2021.
//

#include "ClientMaxBodySize.hpp"
/*
 * ClientMaxBodySize = client_max_body_size 100Mo
 */

ClientMaxBodySize::ClientMaxBodySize() { }

ClientMaxBodySize::result_type ClientMaxBodySize::operator()(const slice &input)
{
	ParserResult<tuple<slice, slice> > res = preceded(
			sequence(Tag("client_max_body_size"), rws),
			sequence(TakeWhile(std::isdigit), opt(alt(Tag("Mo"), Tag("MB"), Tag("M")))))(input);
	if (res.is_ok())
	{
		tuple<slice, slice>	data = res.unwrap();
		size_t size = static_cast<size_t>(data.first.to_int());
		size_t mult = data.second.size ? 1048576 : 1;
		return res.map(size * mult);
	}
	return res.convert<size_t>();
}

/* ************************************************************************** */
