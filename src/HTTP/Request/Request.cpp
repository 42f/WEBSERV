//
// Created by alena on 14/07/2021.
//

#include "Request.hpp"

#include <cstdlib>

#include "RequestUtils/Chunk.hpp"

/*
** ---------------------------------- PRIVATE ----------------------------------
*/
bool Request::receive_chunked(std::vector<char> &buff) {
  bool end = false;

  ParserResult<std::vector<chunk_data> > chunks =
      ChunkBody()(slice(buff.data(), buff.size()));
  if (chunks.is_err()) return false;
  std::vector<chunk_data> lst = chunks.unwrap();
  for (std::vector<chunk_data>::iterator it = lst.begin(); it != lst.end();
       it++) {
    _body.reserve(_body.size() + it->data.size);
    _body.insert(_body.end(), it->data.p, it->data.p + it->data.size);
    if (it->end) end = true;
  }
  buff.erase(buff.begin(), buff.begin() + (chunks.left().p - buff.data()));
  buff.reserve(65550);
  return end;
}

bool Request::receive_raw(std::vector<char> &buff) {
  if (buff.size() >= _length) {
    _body.reserve(_length);
    _body.insert(_body.begin(), buff.begin(), buff.begin() + _length);
    buff.erase(buff.begin(), buff.begin() + _length);
    return true;
  }
  return false;
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Request::Request() : _length(0){};

Request::Request(methods::s_method method, Target target, Version version)
    : _length(0), method(method), target(target), version(version) {}

/*
** --------------------------------- METHODS ----------------------------------
*/
void Request::set_header(const Header &header) {
  this->_headers.insert(std::make_pair(header.name(), header));
}

Result<std::string> Request::get_header(const std::string &name) const {
  std::map<std::string, Header>::const_iterator it = this->_headers.find(name);
  if (it == this->_headers.end())
    return Result<std::string>::err(DefaultError("Unknown cgi"));
  return Result<std::string>::ok(it->second.value());
}

const std::vector<char> &Request::get_body() const { return _body; }
char *Request::get_client_ip(void) const { return _client_ip; }
void Request::set_client_ip(char *client_ip) { _client_ip = client_ip; }

/*
 * Check the type of body and call the corresponding receive handler
 */
bool Request::receive(std::vector<char> &buff) {
  if (_length) return receive_raw(buff);
  if (get_header("Transfer-Encoding").is_ok()) return receive_chunked(buff);
  _length = atoi(get_header("Content-Length").unwrap().c_str());
  return receive_raw(buff);
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/
std::ostream &operator<<(std::ostream &stream, const Request &req) {
  stream << req.method << " " << req.target << " HTTP/" << req.version
         << std::endl;
  for (std::map<std::string, Header>::const_iterator it = req._headers.begin();
       it != req._headers.end(); it++) {
    stream << "|" << it->second << "|" << std::endl;
  }
  stream << std::endl;
  return stream;
}

/* ************************************************************************** */
