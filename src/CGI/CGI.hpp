#ifndef CGI_CGI_HPP
#define CGI_CGI_HPP

#include "File.hpp"

namespace cgi_status {
enum status { READY, ERROR, WAITING };
}

class CGI {
 public:
  CGI(void);
  ~CGI();

  void execute_cgi(void);
  void init(std::string cgi_path, std::string file_path);
  cgi_status::status status(void);
  int get_readable_pipe(void);
  int get_fd(void) const;

 private:
  files::File *_file;
  std::string _cgi_path;
  std::string _file_path;
  int _child_pid;
  int _child_return;
  int _pipe;
  cgi_status::status _status;
};

#endif