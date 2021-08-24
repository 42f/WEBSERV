#ifndef RESPONSE_AUTOINDEX_HPP
#define RESPONSE_AUTOINDEX_HPP

#include <dirent.h>

#include "Response.hpp"

namespace Autoindex {
inline void make(LocationConfig const & loc, std::string const &path, Response &res) {
  DIR *dir;
  struct dirent *ent;
  std::stringstream page;

  page << "<html>" << '\n'
       << "<body>" << '\n'
       << "	<hr>" << '\n'
       << "	<center>Webserv Team ABC</center>" << '\n';
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      std::string file = loc.get_path() + ent->d_name;
      page << "<a href=\"" << file << "\">" << file << "</a></br>" << std::endl;
    }
    closedir(dir);
  }
  page << "</body>" << '\n' << "</html>" << '\n';
  res.getBuffer().assign(page.str());
  return;
}
};  // namespace Autoindex

#endif
