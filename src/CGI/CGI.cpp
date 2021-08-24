#include "CGI.hpp"

CGI::CGI(void) { _status = cgi_status::NON_INIT; }
CGI::~CGI() {}

int CGI::get_fd(void) const { return (_pipe); }

cgi_status::status CGI::status(void) {
  if (_status == cgi_status::DONE || _status == cgi_status::SYSTEM_ERROR ||
      _status == cgi_status::CGI_ERROR) {
    return _status;
  }
  int ret = waitpid(_child_pid, &_child_return, WNOHANG);
  if (ret == _child_pid) {
    if (_child_return < 0) {
      _status = cgi_status::CGI_ERROR;
    } else {
      _status = cgi_status::DONE;
    }
  } else if (ret == 0) {
    _status = cgi_status::READABLE;
  } else if (ret < 0) {
    _status = cgi_status::SYSTEM_ERROR;
  }
  return (_status);
}

int CGI::get_readable_pipe(void) const { return (_pipe); }

std::vector<char *> CGI::set_meta_variables(std::string cgi_path,
                                            files::File const &file,
                                            Request const &req,
                                            LocationConfig const &loc,
                                            config::Server const &serv) {
  RequestLine req_lines;
  std::vector<char *> variables;

  struct stat sb;

  add_variable("AUTH_TYPE", "");
  add_variable("REMOTE_USER", "");
  add_variable("REMOTE_HOST", "");
  add_variable("REMOTE_IDENT", "");
  if (req.target.query.empty()) {
    add_variable("QUERY_STRING", "");
  } else {
    add_variable("QUERY_STRING", req.target.query);
  }
  add_variable("SERVER_SOFTWARE", "");
  add_variable("REDIRECT_STATUS", "200");
  add_variable("PATH_INFO", file.getPath());
  add_variable("SCRIPT_NAME", file.getFileFromPath(file.getPath()));
  add_variable("SERVER_PROTOCOL", "HTTP/1.1");
  add_variable("GATEWAY_INTERFACE", "CGI/1.1");
  add_variable("SERVER_PORT", serv.get_port());
  add_variable("SERVER_NAME", serv.get_name());
  add_variable("REMOTE_ADDR", req.get_client_ip());
  // variables.push_back("SCRIPT_NAME=cgi_info.php");
  add_variable("PATH_TRANSLATED", file.getPath());
  add_variable("SCRIPT_FILENAME", file.getPath());
  if (req.method == methods::GET)
    add_variable("REQUEST_METHOD", "GET");
  else if (req.method == methods::POST)
    add_variable("REQUEST_METHOD", "POST");
  else if (req.method == methods::DELETE)
    add_variable("REQUEST_METHOD", "DELETE");
  else {
    _status = cgi_status::UNSUPPORTED;
    return variables;
  }
  Result<std::__1::string> content_length = req.get_header("content-length");
  if (content_length.is_ok()) {
    std::cout << "content length: " << content_length.unwrap() << std::endl;
    add_variable("CONTENT_LENGTH", content_length.unwrap());
  } else {
    add_variable("CONTENT_LENGTH", "");
  }
  Result<std::__1::string> content_type = req.get_header("content-type");
  if (content_type.is_ok()) {
    add_variable("CONTENT_TYPE", content_type.unwrap());
  } else {
    add_variable("CONTENT_TYPE", "");
  }

  return variables;
}

void CGI::execute_cgi(std::string cgi_path, files::File const &file,
                      Request const &req, LocationConfig const &loc,
                      config::Server const &serv) {

  
  _status = cgi_status::NON_INIT;
  int pipes[2];

  int i = 0;
  set_meta_variables(cgi_path, file, req, loc, serv);

  char *env[_variables.size() + 1];
  for (; i < _variables.size();) {
    env[i] = _variables[i];
    i++;
  }
  env[i] = NULL;

  char *cgi = strdup(cgi_path.c_str());
  if (cgi == NULL || file.isGood() == false) {
    _status = cgi_status::SYSTEM_ERROR;
    return;
  }
  char *args[] = {cgi, NULL};

  if (pipe(pipes) < 0) {
    perror("System Error : pipe()");
    _status = cgi_status::SYSTEM_ERROR;
    return;
  }

  _child_pid = fork();
  if (_child_pid < 0) {
    perror("System Error : fork()");
    _status = cgi_status::SYSTEM_ERROR;
    return;
  }
  if (_child_pid == 0) {
    std::string exec_path = files::File::getDirFromPath(file.getPath());
    std::cout << "executing CGI from : " << exec_path << std::endl;
    close(pipes[0]);
    if (dup2(pipes[1], 1) < 0) {
      perror("System Error : dup2()");
      _status = cgi_status::SYSTEM_ERROR;
      return;
    }
    close(pipes[1]);
    chdir(exec_path.c_str());
    if (execve(args[0], args, env) < 0) {
      exit(1);
    }
  } else {
    waitpid(_child_pid, &_child_return, WNOHANG);
    close(pipes[1]);
    _pipe = pipes[0];
    _status = cgi_status::READABLE;
  }
}
