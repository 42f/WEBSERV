#ifndef CGI_CGI_HPP
#define CGI_CGI_HPP

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <vector>

#include "Config/Server.hpp"
#include "File.hpp"
#include "HTTP/Request/Request.hpp"

namespace cgi_status {
enum status { DONE, ERROR, READABLE };
}

class CGI {
 public:
  CGI(void);
  ~CGI();

  void execute_cgi(void);
  void execute_cgi(std::string cgi_path, files::File const &file,
                   Request const &req, LocationConfig const &loc,
                   config::Server const &serv);
  cgi_status::status status(void);
  int get_readable_pipe(void) const;
  int get_fd(void) const;

 private:
  files::File *_file;
  std::string _cgi_path;
  std::string _file_path;
  int _child_pid;
  int _child_return;
  int _pipe;
  cgi_status::status _status;

  std::vector<char const *> set_meta_variables(std::string cgi_path,
                                               files::File const &file,
                                               Request const &req,
                                               LocationConfig const &loc,
                                               config::Server const &serv);
};

#endif