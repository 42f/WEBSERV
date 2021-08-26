//
// Created by alena on 14/07/2021.
//

#include "Redirect.hpp"

/*
 * struct de Redirect
 */
redirect::redirect() : status(status::None), uri("") {}
redirect::redirect(int code, std::string uri)
    : status(static_cast<status::StatusCode>(code)), uri(uri) {}
redirect::redirect(status::StatusCode code, std::string uri)
    : status(code), uri(uri) {}

redirect redirect::parse(tuple<slice, slice> input) {
  return redirect(input.first.to_int(), input.second.to_string());
}

std::string redirect::resolveRedirect(Target const& target) const {
	static const std::string params[] = {
		PARAM_REDIR_REQ_SCHEME, PARAM_REDIR_REQ_URI, PARAM_REDIR_REQ_QUERY
	};
	const std::string *values[] = { &target.scheme, &target.path, &target.query };

	std::string redirUri(uri);
	size_t pos = 0;
	while (pos != std::string::npos) {
		int param = -1;
		size_t rpos = redirUri.find(PARAM_REDIR_REQ_SCHEME, pos);
		size_t upos = redirUri.find(PARAM_REDIR_REQ_URI, pos);
		size_t qpos = redirUri.find(PARAM_REDIR_REQ_QUERY, pos);
		if ((pos = rpos) != std::string::npos)
			param = 0;
		if (pos == std::string::npos || (upos != std::string::npos && upos < pos)) {
			pos = upos;
			param = 1;
		}
		if (pos == std::string::npos || (qpos != std::string::npos && qpos < pos)) {
			pos = qpos;
			param = 2;
		}
		if (pos != std::string::npos && param != -1)
		{
			redirUri.replace(pos, params[param].length(), *values[param]);
			pos += values[param]->length();
		}
	}
	return redirUri;
}

/*
 * Redirect = return digit path
 */
Redirect::Redirect() {}

Redirect::result_type Redirect::operator()(const slice& input) {
  return map(preceded(sequence(Tag("return"), rws),
                      sequence(TakeWhile(std::isdigit),
                               preceded(rws, TakeUntil(";")))),
             redirect::parse)(input);
}

/* ************************************************************************** */
