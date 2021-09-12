#include "CGI.hpp"

CGI::CGI(void) {
  _status = cgi_status::NON_INIT;
  _pipe = UNSET;
  _child_return = 0;
}
CGI::~CGI() {
  if (_pipe != UNSET) close(_pipe);
  if (_status != cgi_status::NON_INIT)
    waitpid(_child_pid, &_child_return, WNOHANG);
}

int CGI::get_pid(void) const { return (_child_pid); }
int CGI::get_fd(void) const { return (_pipe); }

bool CGI::isPipeEmpty(void) const {
  unsigned long bytesAvailable;
  if (ioctl(_pipe, FIONREAD, &bytesAvailable) == -1) {
    perror("iotctl");
    bytesAvailable = 0;
  }
  return bytesAvailable == 0;
}

cgi_status::status CGI::status(void) {
  if (_cgiTimer.getTimeElapsed() >= CGI_TIMEOUT) {
    _status = cgi_status::TIMEOUT;
    return _status;
  }

  if (_status == cgi_status::DONE || STATUS_IS_ERROR(_status)) {
    return _status;
  }

  int ret = waitpid(_child_pid, &_child_return, WNOHANG);

  if (ret == _child_pid) {
    if (CGI_BAD_EXIT(_child_return)) {
      if (WIFSIGNALED(_child_return) || WEXITSTATUS(_child_return) == 255) {
        _status = cgi_status::SYSTEM_ERROR;
      } else {
        _status = cgi_status::CGI_ERROR;
      }
    } else {
      _status = cgi_status::DONE;
    }
  } else if (ret == 0 && isPipeEmpty() == false) {
    _status = cgi_status::READABLE;
  } else if (ret == 0 && isPipeEmpty() == true) {
    _status = cgi_status::WAITING;
  } else if (ret < 0) {
    _status = cgi_status::SYSTEM_ERROR;
  }
  return (_status);
}

std::string const &CGI::getCgiHeader(void) const { return _cgiHeaders; }

void CGI::setCgiHeader(void) {
  if (_cgiHeaders.empty() && isPipeEmpty() == false) {
    char cBuff;
    int retRead = 1;
    while ((retRead = read(_pipe, &cBuff, 1)) > 0) {
      _cgiHeaders += cBuff;
      if (_cgiHeaders.size() >= 3 &&
          _cgiHeaders[_cgiHeaders.length() - 3] == '\n' &&
          _cgiHeaders[_cgiHeaders.length() - 2] == '\r' &&
          _cgiHeaders[_cgiHeaders.length() - 1] == '\n')
        break;
    }
  }
}

int CGI::get_readable_pipe(void) const { return (_pipe); }

std::vector<char *> CGI::set_meta_variables(files::File const &file,
                                            Request const &req,
                                            config::Server const &serv) {
  RequestLine req_lines;
  std::vector<char *> variables;

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
  add_variable("REDIRECT_STATUS", "");
  add_variable("PATH_INFO", file.getFileName());
  // add_variable("SCRIPT_NAME", "");//file.getFileFromPath(file.getPath()));
  add_variable("SERVER_PROTOCOL", "HTTP/1.1");
  add_variable("GATEWAY_INTERFACE", "CGI/1.1");
  add_variable("SERVER_PORT", serv.get_port());
  add_variable("SERVER_NAME", serv.get_name());
  add_variable("REMOTE_ADDR", req.get_client_ip());
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

  add_variable("CONTENT_LENGTH",
               req.get_header("Content-Length").unwrap_or(""));
  add_variable("CONTENT_TYPE", req.get_header("Content-Type").unwrap_or(""));

  return variables;
}

void CGI::execute_cgi(std::string const &cgi_path, files::File const &file,
                      Request const &req, config::Server const &serv) {
  _status = cgi_status::NON_INIT;
  int output[2];
  int input[2];

  size_t i = 0;
  set_meta_variables(file, req, serv);

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
  pipe(output);
  pipe(input);

  _child_pid = fork();
  if (_child_pid < 0) {
    perror("System Error : fork()");
    _status = cgi_status::SYSTEM_ERROR;
    return;
  }
  if (_child_pid == 0) {
    dup2(output[1], 1);
    close(input[1]);
    close(input[0]);

    dup2(input[0], 0);
    close(output[1]);
    close(output[0]);

    execve(args[0], args, env);
    exit(-1);
  } else {
    _cgiTimer.start();
    _status = cgi_status::WAITING;
    int ret;
    if (req.get_body().size() > 0)
      ret = write(input[1], req.get_body().data(), req.get_body().size());
    std::cout << "write to child process " << ret << " bytes" << std::endl; // TODO remove
    close(output[1]);
    close(input[0]);
    close(input[1]);
    _pipe = output[0];
    // fcntl(_pipe, F_SETFL, O_NONBLOCK); // TODO necessary ??
    free(cgi);
    for (i = 0; i < _variables.size(); i++) {
      free(env[i]);
    }
  }
}
