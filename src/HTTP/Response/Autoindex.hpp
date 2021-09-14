#ifndef RESPONSE_AUTOINDEX_HPP
#define RESPONSE_AUTOINDEX_HPP

#include <dirent.h>

#include "ResponseHandler.hpp"

namespace Autoindex {

inline std::string humanReadable(off_t inputSize) {
  std::stringstream output;
  if (inputSize > 1000000) {
    output.precision(2);
    double roundedSize = inputSize;
    output << std::fixed << roundedSize / 1000000 << " Mb";
  } else if (inputSize > 1000) {
    output.precision(2);
    double roundedSize = inputSize;
    output << std::fixed << roundedSize / 1000 << " Kb";
  } else {
    output << inputSize;
  }
  return output.str();
}

inline void make(std::string const &target, std::string const &path,
                 Response &res) {
  DIR *dir;
  struct dirent *ent;
  std::stringstream page;

  if ((dir = opendir(path.c_str())) != NULL) {
    std::map<std::string, std::string> filesTree;
    struct stat st;
    while ((ent = readdir(dir)) != NULL) {
      std::string file(ent->d_name);
      if (file == "." || file == "..") continue;
      std::stringstream line;
      stat(std::string(path + file).c_str(), &st);

      line << "<div class=\"flex-container\">" << std::endl
           << "<div_file>"
           << "<a href=\"" << target << (target.empty() ? "/" : "") << file
           << "\">" << file << "</a></div_file>" << std::endl;
      if (S_ISDIR(st.st_mode))
        line << "<div> - </div>" << std::endl;
      else
        line << "<div>" << humanReadable(st.st_size) << "</div>" << std::endl;
      line << "<div>" << Timer::getTimeStr(gmtime(&(st.st_mtime))) << "</div>"
           << std::endl
           << "</div>" << std::endl;
      filesTree[file] = line.str();
    }
    closedir(dir);

    page << "<!doctype html>" << std::endl
         << "" << std::endl
         << "<html lang=\"en\">" << std::endl
         << "<head>" << std::endl
         << "<meta charset=\"utf-8\">" << std::endl
         << "<meta name=\"viewport\" content=\"width=device-width, "
            "initial-scale=1\">"
         << std::endl
         << "" << std::endl
         << "<style>" << std::endl
         << ".flex-container {" << std::endl
         << "  display: flex;" << std::endl
         << "  width: 75%;" << std::endl
         << "  /* background-color: DodgerBlue; */" << std::endl
         << "}" << std::endl
         << "" << std::endl
         << ".flex-container > div_file {" << std::endl
         << " word-wrap: break-word;" << std::endl
         << "	font-family: 'Courier New', Courier, monospace;" << std::endl
         << "	width: 42%;" << std::endl
         << "  /* background-color: #f1f1f1; */" << std::endl
         << "  margin-left: 25px;" << std::endl
         << "  text-align: left;" << std::endl
         << "  font-roundedSize: 15px;" << std::endl
         << "}" << std::endl
         << ".flex-container > div {" << std::endl
         << "	font-family: 'Courier New', Courier, monospace;" << std::endl
         << "	width: 26%;" << std::endl
         << "  /* background-color: #f1f1f1; */" << std::endl
         << "  margin-left: 25px;" << std::endl
         << "  text-align: left;" << std::endl
         << "  font-size: 15px;" << std::endl
         << "}" << std::endl
         << "</style>" << std::endl
         << "" << std::endl
         << "" << std::endl
         << "<title>Index of " << target << "</title>" << std::endl
         << "<meta name=\"description\" content=\"Index\">" << std::endl
         << "<meta name=\"author\" content=\"team ABC\">" << std::endl
         << "" << std::endl
         << "<meta property=\"og:title\" content=\"Index Page of " << target
         << "\">" << std::endl
         << "<meta property=\"og:type\" content=\"website\">" << std::endl
         << "<meta property=\"og:description\" content=\"Autoindex\">"
         << std::endl
         << "" << std::endl
         << "<link rel=\"icon\" href=\"/imgs/favicon.ico\">" << std::endl
         << "" << std::endl
         << "" << std::endl
         << "</head>" << std::endl
         << "" << std::endl
         << "" << std::endl
         << "<body>" << std::endl
         << "<h1>Index of " << target << "</h1>" << std::endl
         << "<div class=\"flex-container\">" << std::endl
         << "<div_file><a href=\"..\">../</a></div_file> </div>" << std::endl;

    std::map<std::string, std::string>::iterator it = filesTree.begin();
    for (; it != filesTree.end(); it++) {
      page << it->second;
    }
  }
  page << "</body>" << std::endl << "</html>" << std::endl;
  res.getBuffer().assign(page.str());
}

};  // namespace Autoindex

#endif
