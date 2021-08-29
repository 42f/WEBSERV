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
       << "<head>" << '\n'
       << "<meta charset=\"utf-8\">" << '\n'
       << "<meta name=\"viewport\" content=\"width=device-width, initial-scale "
          "=1\">"
       << '\n'
       << "<style>" << '\n'
       << "h1 {" << '\n'
       << "text-align: center;" << '\n'
       << "font-family: arial, sans-serif;" << '\n'
       << "}" << '\n'
       << "" << '\n'
       << "" << '\n'
       << ".link_container {" << '\n'
       << "display: flex;" << '\n'
       << "flex-direction: column;" << '\n'
       << "justify-content: center;" << '\n'
       << "align-items: center;" << '\n'
       << "}" << '\n'
       << "" << '\n'
       << ".link_container li {" << '\n'
       << "list-style-type: none;" << '\n'
       << "/* border: 1px solid red; */" << '\n'
       << "width: 100%;" << '\n'
       << "}" << '\n'
       << "" << '\n'
       << ".link_container a {" << '\n'
       << "display: block;" << '\n'
       << "margin: 2px 0 2px 0;" << '\n'
       << "width: 70%;" << '\n'
       << "border: 1px solid rgb(173, 173, 173);" << '\n'
       << "padding: 7px;" << '\n'
       << "}" << '\n'
       << "" << '\n'
       << "</style>" << '\n'
       << "<title>ABOUT</title>" << '\n'
       << "<meta name=\"description\" content=\"Coucou\">" << '\n'
       << "<meta name=\"author\" content=\"team ABC\">" << '\n'
       << "" << '\n'
       << "<meta property=\"og:title\" content=\" A Basic HTML5 Template \">"
       << '\n'
       << "<meta property=\"og : type\" content=\"website\">" << '\n'
       << "<meta property=\"og : description\" content=\"A webserv\">" << '\n'
       << "<meta property=\"og : image\" content=\"image.png\">" << '\n'
       << "" << '\n'
       << "<link rel=\"icon\" href=\"/imgs/favicon.ico\">" << '\n'
       << "" << '\n'
       << "" << '\n'
       << "</head>" << '\n'
       << "<body>" << '\n'
       << "	<hr>" << '\n'
       << "	<center>Webserv Team ABC</center>" << '\n'
       << "	<ul class=\"link_container\">";
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      std::string file = ent->d_name;
      page << "<li><a href=\"" << target << (target.empty() ? "/" : "") << file
           << "\">" << file << "</a></li></br>" << std::endl;
    }
    closedir(dir);
  }
  page << "</ul>"
       << "</body>" << '\n'
       << "</html>" << '\n';
  res.getBuffer().assign(page.str());
  return;
}
};  // namespace Autoindex

#endif
