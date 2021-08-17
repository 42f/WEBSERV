#include "CGI.hpp"

CGI::CGI(void) { _status = cgi_status::ERROR; }
CGI::~CGI() {}

void CGI::init(std::string cgi_path, std::string file_path) {
  _cgi_path = cgi_path;
  _file_path = file_path;
  _status = cgi_status::ERROR;
}

int CGI::get_fd(void) const { return (_pipe); }

cgi_status::status CGI::status(void) {
  int ret = waitpid(_child_pid, &_child_return, WNOHANG);
  if (ret == _child_pid) {
    _status = cgi_status::READY;
  } else if (ret == 0) {
    _status = cgi_status::WAITING;
  } else if (ret < 0) {
    _status = cgi_status::ERROR;
  }
  return (_status);
}

void CGI::execute_cgi(void) {
  int pipes[2];

  // This line will be removed. The File's fd will be used directly
  int fd = open(_file_path.c_str(), O_RDONLY);

  char *GATEWAY_INTERFACE = "CGI/1.1";
  char *SERVER_PROTOCOL = "HTTP/1.1";
  char *SCRIPT_FILENAME = "/Users/calide-n/Documents/web/info.php";
  char *SCRIPT_NAME = "info.php";
  char *REDIRECT_STATUS = "200";

  char *env[] = {GATEWAY_INTERFACE, SERVER_PROTOCOL, SCRIPT_FILENAME,
                 SCRIPT_NAME, REDIRECT_STATUS, NULL};


  // /Users/calide-n/.brew/bin/php-cgi
  char *cgi = strdup(_cgi_path.c_str());
  char *args[] = {cgi, NULL};

  if (pipe(pipes) < 0) {
    perror("pipes");
    return;
  }

  _child_pid = fork();
  if (_child_pid < 0) {
    perror("fork");
    // return 501 ?
  }
  if (_child_pid == 0) {
    dup2(fd, 0);
    dup2(pipes[1], 1);
    close(fd);
    close(pipes[0]);
    execve(args[0], args, env);
    exit(0);
  } else {
    waitpid(_child_pid, &_child_return, WNOHANG);
    close(pipes[1]);
    _pipe = pipes[0];
  }
}