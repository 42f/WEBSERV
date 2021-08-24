//
// Created by alena on 14/06/2021.
//

#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include <map>

#include "Config/Directives/Redirect.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "RequestLine.hpp"

/*
 * Request
 */
struct Request {
 private:
  std::map<std::string, Header> _headers;
  std::vector<char> _body;
  size_t _length;
  char *_client_ip;

  bool receive_chunked(std::vector<char> &buff);
  bool receive_raw(std::vector<char> &buff);

 public:
  methods::s_method method;
  Target target;
  Version version;

  Request();
  Request(methods::s_method method, Target target, Version version);

  void set_header(const Header &header);

  Result<std::string> get_header(const std::string &name) const;
  const std::vector<char> &get_body() const;
  char *get_client_ip(void) const;
  void set_client_ip(char *client_ip);

  bool receive(std::vector<char> &vector);

  friend std::ostream &operator<<(std::ostream &stream, const Request &req);
};

/* ************************************************************************** */

#endif  // WEBSERV_REQUEST_HPP
