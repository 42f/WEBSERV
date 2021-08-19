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

  // on macbook pro
  // _cgi_path = "/usr/local/bin/php-cgi";

  // on mac 42
  _cgi_path = "/Users/calide-n/.brew/bin/php-cgi";

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
  char *REQUEST_METHOD = "REQUEST_METHOD=GET";

  char *AUTH_TYPE = "AUTH_TYPE=";
  // Lenght of the request body, can be null or unset
  char *CONTENT_LENGTH = "CONTENT_LENGTH=";
  // Content type of the request body
  char *CONTENT_TYPE = "CONTENT_TYPE=";
  // End of PATH_INFO
  char *PATH_TRANSLATED = "PATH_TRANSLATED=/cgi_info.php";
  // Arguments for the script
  char *QUERY_STRING = "QUERY_STRING=\"\"";
  char *REMOTE_ADDR = "REMOTE_ADDR=";
  char *REMOTE_HOST = "REMOTE_HOST=";
  char *REMOTE_IDENT = "REMOTE_IDENT=";
  char *REMOTE_USER = "REMOTE_USER=";
  char *SERVER_NAME = "SERVER_NAME=";
  char *SERVER_PORT = "SERVER_PORT=";
  char *SERVER_SOFTWARE = "SERVER_SOFTWARE=";

  char *env[] = {
      GATEWAY_INTERFACE, SERVER_PROTOCOL, SCRIPT_FILENAME, SCRIPT_NAME,
      REDIRECT_STATUS,   PATH_INFO,       REQUEST_METHOD,  CONTENT_TYPE,
      AUTH_TYPE,         CONTENT_LENGTH,  PATH_TRANSLATED, QUERY_STRING,
      REMOTE_ADDR,       REMOTE_HOST,     REMOTE_IDENT,    REMOTE_USER,
      SERVER_NAME,       SERVER_PORT,     SERVER_SOFTWARE, NULL};

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
    // dup2(pipes[1], 1);
    close(pipes[1]);
    if (execve(args[0], args, env) < 0) {
		// std::cout << "error" << std::endl;
		// perror("execve");
	}
    exit(1);
  } else {
    waitpid(_child_pid, &_child_return, WNOHANG);
    _status = cgi_status::READABLE;
    close(pipes[1]);
    _pipe = pipes[0];
  }
}
