#ifndef RESPONSE_AUTOINDEX_HPP
#define RESPONSE_AUTOINDEX_HPP

#include <dirent.h>
#include "ResponseHandler.hpp"

namespace Autoindex {
inline void make(std::string const &target, std::string const &path,
                 Response &res) {
  DIR *dir;
  struct dirent *ent;
  std::stringstream page;
  page << "<html>" << '\n'
       << "<body>" << '\n'
       << "	<hr>" << '\n'
       << "	<center>Webserv Team ABC</center>" << '\n';
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      std::string file = ent->d_name;
      page << "<a href=\"" << target << (target.empty() ? "/" : "") << file
           << "\">" << file << "</a></br>" << std::endl;
    }
    closedir(dir);
  }
  page << "</body>" << '\n' << "</html>" << '\n';
  res.getBuffer().assign(page.str());
  return;
}
};  // namespace Autoindex

#endif
