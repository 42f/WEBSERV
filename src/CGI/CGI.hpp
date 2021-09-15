#ifndef CGI_CGI_HPP
#define CGI_CGI_HPP

#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <vector>

#include "Config/Server.hpp"
#include "HTTP/Response/ResponseUtils/File.hpp"
#include "HTTP/Request/Request.hpp"
#include "Timer.hpp"
#include "Constants.hpp"

#define STATUS_IS_ERROR(x) \
    (x == cgi_status::CGI_ERROR || x == cgi_status::SYSTEM_ERROR \
  || x == cgi_status::UNSUPPORTED || x == cgi_status::TIMEOUT)

#define CGI_BAD_EXIT(x) \
    (WIFEXITED(x) &&  WEXITSTATUS(x) != 0) || WIFSIGNALED(x)

namespace cgi_status {
  enum status {
    NON_INIT,
    WAITING,
    DONE,
    CGI_ERROR,
    SYSTEM_ERROR,
    READABLE,
    UNSUPPORTED,
    TIMEOUT
  };
}

class CGI {
 public:
  CGI(void);
  ~CGI();

  void execute_cgi(void);
  int execute_cgi(std::string const &cgi_path, files::File const &file,
                   Request const &req, config::Server const &serv);
  cgi_status::status status(void);
  int get_readable_pipe(void) const;
  int get_pid(void) const;
  int get_fd(void) const;
  void setCgiHeader(void);
  std::string const & getCgiHeader(void) const;

 private:
  files::File *_file;
  int _child_pid;
  int _child_return;
  int _pipe;
  cgi_status::status _status;
  std::vector<char *> _variables;
  std::string _cgiHeaders;
  Timer    _cgiTimer;

  void onReturn(char *cgi, char *env);
  bool isPipeEmpty(int fd) const;
  template <typename T>
  void add_variable(std::string name, T value) {
    std::ostringstream ss;
    ss << name << "=" << value;
    _variables.push_back(strdup(ss.str().c_str()));
  }

  std::vector<char *> set_meta_variables(files::File const &file,
                                         Request const &req,
                                         config::Server const &serv);
};

#endif
