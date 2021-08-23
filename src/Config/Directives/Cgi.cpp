//
// Created by alena on 14/07/2021.
//

#include "Cgi.hpp"

/*
 * Cgi = cgi .extension Path
 */

Cgi::Cgi() {}

Cgi::result_type Cgi::operator()(const slice &input) {
  result_type result =
      sequence(delimited(sequence(Tag("cgi"), rws),
                         preceded(Tag("."), TakeUntil(" ")), rws),
               TakeUntil(";"))(input);

  if (result.is_ok()) {
    std::string cgi_path = result.unwrap().second.to_string();
    if (access(cgi_path.c_str(), X_OK))
    {
      return result_type::fail(
          result.left(), error("CGI path does not exist or is non executable by the current user"));
    }
  }
  return result;
}

/* ************************************************************************** */
