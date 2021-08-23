#ifndef RESPONSE_AUTOINDEX_HPP
#define RESPONSE_AUTOINDEX_HPP

#include "HTTP/Request/Request.hpp"
#include "Response.hpp"
#include <dirent.h>
//#include <sys/stat.h>

class Autoindex {
 public:
  static void make(std::string &path, Response &res);

};

#endif