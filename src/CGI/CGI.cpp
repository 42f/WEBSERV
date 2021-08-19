#include "CGI.hpp"

CGI::CGI(void) { _status = cgi_status::ERROR; }
CGI::~CGI() {}

int CGI::get_fd(void) const { return (_pipe); }

cgi_status::status CGI::status(void) {
  if (_status == cgi_status::DONE) {
    return _status;
  }
  int ret = waitpid(_child_pid, &_child_return, WNOHANG);
  if (ret == _child_pid) {
    std::cout << "status is done" << std::endl;
    _status = cgi_status::DONE;
  } else if (ret == 0) {
    std::cout << "status is readable" << std::endl;
    _status = cgi_status::READABLE;
  } else if (ret < 0) {
    std::cout << "status is error" << std::endl;
    _status = cgi_status::ERROR;
    // exit(1);
  }
  return (_status);
}

int CGI::get_readable_pipe(void) const { return (_pipe); }

void CGI::execute_cgi(std::string cgi_path, files::File const &file) {
  _cgi_path = cgi_path;
  _file_path = file.getPath();
  _status = cgi_status::ERROR;
  int pipes[2];

  _cgi_path = "/usr/local/bin/php-cgi";

  struct stat sb;

  if (stat(_file_path.c_str(), &sb) == -1) {
    perror("stat");
    return;
  }

  bzero(&sb, sizeof(sb));

  if (stat(_cgi_path.c_str(), &sb) == -1) {
    perror("stat");
    return;
  }

  _file_path = "PATH_INFO=" + _file_path;

  char *GATEWAY_INTERFACE = "GATEWAY_INTERFACE=CGI/1.1";
  char *SERVER_PROTOCOL = "SERVER_PROTOCOL=HTTP/1.1";
  char *SCRIPT_FILENAME = "SCRIPT_FILENAME=/tmp/server/0/cgi_info.php";
  char *SCRIPT_NAME = "SCRIPT_NAME=cgi_info.php";
  char *REDIRECT_STATUS = "REDIRECT_STATUS=200";
  char *PATH_INFO = strdup(_file_path.c_str());

  char *env[] = {GATEWAY_INTERFACE,
                 SERVER_PROTOCOL,
                 SCRIPT_FILENAME,
                 SCRIPT_NAME,
                 REDIRECT_STATUS,
                 PATH_INFO,
                 NULL};

  // /Users/calide-n/.brew/bin/php-cgi
  char *cgi = strdup(_cgi_path.c_str());
  char *file_path = strdup(_file_path.c_str());
  char *args[] = {cgi, NULL};

  if (pipe(pipes) < 0) {
    perror("pipes");
    // return 501 ?
  }

  _child_pid = fork();
  if (_child_pid < 0) {
    perror("fork");
    // return 501 ?
  }
  if (_child_pid == 0) {
    close(pipes[0]);
    dup2(pipes[1], 1);
    close(pipes[1]);
    execve(args[0], args, env);
    // exit(0);
  } else {
    waitpid(_child_pid, &_child_return, WNOHANG);
    _status = cgi_status::READABLE;
    close(pipes[1]);
    _pipe = pipes[0];
  }
}