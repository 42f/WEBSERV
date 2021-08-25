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
  std::string redirUri(uri);
  const char* params[] = {PARAM_REDIR_REQ_SCHEME, PARAM_REDIR_REQ_URI,
                          PARAM_REDIR_REQ_QUERY};
  const char* newVal[] = {target.scheme.c_str(), target.path.c_str(),
                          target.query.c_str()};

  size_t paramPos = 0;
  for (int i = 0; i < sizeof(params) / sizeof(char**); i++) {
    paramPos = 0;
    while ((paramPos = redirUri.find(params[i]), paramPos) !=
           std::string::npos) {
      redirUri.replace(paramPos, strlen(params[i]), newVal[i]);
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
