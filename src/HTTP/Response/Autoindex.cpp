#include "Autoindex.hpp"

void Autoindex::make(std::string &path, Response &res) {
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
      std::string full_path = path + file;
      page << "<a url=\"google.com\">" << file << "</a></br>"
                << std::endl;
      //   struct stat path_stat;
      //   stat(full_path.c_str(), &path_stat);
      //   if (S_ISDIR(path_stat.st_mode) && file != "." && file != "..") {
      //   }
    }
    closedir(dir);
  }
  page << "</body>" << '\n' << "</html>" << '\n';
  res.getBuffer().assign(page.str());
  return;
}
