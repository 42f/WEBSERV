//
// Created by alena on 14/07/2021.
//

#include "Root.hpp"

/*
 * Root = root RWS path
 */
Root::Root() { }

Root::result_type Root::operator()(const slice &input)
{
	result_type res = preceded(
			sequence(Tag("root"), rws),
			TakeUntil(";"))(input);
	if (res.is_ok())
	{
		std::string 	path = res.unwrap().to_string();
		struct stat st;
		if (stat(path.c_str(), &st) || !S_ISDIR(st.st_mode))
			return result_type::fail(res.left(), error("invalid root directory"));
	}
	return res;
}

/* ************************************************************************** */
