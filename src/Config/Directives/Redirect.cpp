//
// Created by alena on 14/07/2021.
//

#include "Redirect.hpp"

/*
 * struct de Redirect
 */
redirect::redirect() : status(0), uri("") { }
redirect::redirect(int code, std::string uri): status(code), uri(uri) { }

redirect	redirect::parse(tuple<slice, slice> input)
{
	return redirect(input.first.to_int(), input.second.to_string());
}

/*
 * Redirect = return digit path
 */
Redirect::Redirect() { }

Redirect::result_type Redirect::operator()(const slice &input)
{
	return map(preceded(sequence(Tag("return"), rws),
						sequence(TakeWhile(std::isdigit),
								 preceded(rws, TakeUntil(";")))), redirect::parse)(input);
}

/* ************************************************************************** */
