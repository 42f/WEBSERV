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
enum status { NON_INIT, DONE, CGI_ERROR, SYSTEM_ERROR, READABLE, UNSUPPORTED };
}

class CGI {
 public:
  CGI(void);
  ~CGI();

  void execute_cgi(void);
  void execute_cgi(std::string const &cgi_path, files::File const &file,
                   Request const &req, config::Server const &serv);
  cgi_status::status status(void);
  int get_readable_pipe(void) const;
  int get_fd(void) const;

 private:
  files::File *_file;
  int _child_pid;
  int _child_return;
  int _pipe;
  cgi_status::status _status;
  std::vector<char *> _variables;

  template <typename T>
  void add_variable(std::string name, T value) {
    std::ostringstream ss;
    ss << name << "=" << value;
    _variables.push_back(strdup(ss.str().c_str()));
  }

  std::vector<char *> set_meta_variables(std::string const &cgi_path,
                                         files::File const &file,
                                         Request const &req,
                                         config::Server const &serv);
};

#endif
