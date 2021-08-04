//
// Created by alena on 14/07/2021.
//

#include "Methods.hpp"

/*
 * Method = method GET POST ...
 * sequence(slice, many(preceded(Char(" "), slice))
 */
namespace config
{
	config::Methods::Methods() { }

	config::Methods::result_type config::Methods::operator()(const slice &input)
	{
		ParserResult<tuple<methods::s_method, std::vector<methods::s_method> > > res = preceded(
				sequence(Tag("methods"), rws), sequence(RequestMethod(), many(
						preceded(Char(' '), RequestMethod()), true)))(input); //TODO I DONT UNDERTANCE
		if (res.is_ok())
		{
			methods::Methods ret = methods::Methods(res.unwrap().first);
			std::vector<methods::s_method> lst = res.unwrap().second;
			for (std::vector<methods::s_method>::const_iterator it = lst.begin(); it != lst.end(); it++) {
				ret.add_method(*it);
			}
			return res.map(ret);
		}
		return res.convert<methods::Methods>();
	}
}

/* ************************************************************************** */
