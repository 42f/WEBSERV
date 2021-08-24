//
// Created by alena on 10/07/2021.
//

#ifndef WEBSERV_CGI_HPP
#define WEBSERV_CGI_HPP

#include <unistd.h>

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

/*
 * Cgi = cgi .extension Path
 */
class Cgi : public Parser<tuple<slice, slice> > {
 public:
  Cgi();

  result_type operator()(const slice &input);
};

/* ************************************************************************** */

#endif  // WEBSERV_CGI_HPP
