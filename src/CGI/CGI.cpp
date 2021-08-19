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

std::vector<char const *> CGI::set_meta_variables(std::string cgi_path,
                                                  files::File const &file,
                                                  Request const &req,
                                                  LocationConfig const &loc,
                                                  config::Server const &serv) {
  RequestLine req_lines;
  std::vector<char const *> variables;

  _cgi_path = cgi_path;
  _file_path = file.getPath();
  _status = cgi_status::ERROR;

  std::cout << "req target" << std::endl;
  std::cout << "req target deco path : " << req.target.decoded_path
            << std::endl;
  std::cout << "req target deco query : " << req.target.decoded_query
            << std::endl;
  std::cout << "req target path : " << req.target.path << std::endl;
  std::cout << "req target query : " << req.target.query << std::endl;
  std::cout << "req target scheme : " << req.target.scheme << std::endl;

  // on macbook pro
  // _cgi_path = "/usr/local/bin/php-cgi";
  // on mac 42
  // /Users/calide-n/.brew/bin/php-cgi
  // _cgi_path = "/Users/calide-n/.brew/bin/php-cgi";

  struct stat sb;

  // not useful since error 404
  if (stat(_file_path.c_str(), &sb) == -1) {
    perror("stat");
  }

  bzero(&sb, sizeof(sb));

  // TODO : finish error management here
  if (stat(_cgi_path.c_str(), &sb) == -1) {
    perror("stat");
    _status = cgi_status::ERROR;
  }

  // Path of the file to execute
  _file_path = "PATH_INFO=" + _file_path;
  variables.push_back(strdup(_file_path.c_str()));
  // Gateway interace protocol
  variables.push_back("GATEWAY_INTERFACE=CGI/1.1");
  // Server protocol
  variables.push_back("SERVER_PROTOCOL=HTTP/1.1");
  // ?
  // variables.push_back("SCRIPT_FILENAME=/tmp/server/0/cgi_info.php");
  variables.push_back("SCRIPT_FILENAME=/tmp/server/0/cgi_info.php");
  // ?
  // variables.push_back("SCRIPT_NAME=cgi_info.php");
  variables.push_back("SCRIPT_NAME=cgi_info.php");
  // ?
  variables.push_back("REDIRECT_STATUS=200");
  // Request method (GET/POST/DELETE)
  if (req.method == methods::GET)
    variables.push_back("REQUEST_METHOD=GET");
  else if (req.method == methods::POST)
    variables.push_back("REQUEST_METHOD=POST");
  else if (req.method == methods::DELETE)
    variables.push_back("REQUEST_METHOD=DELETE");
  else {
    _status = cgi_status::ERROR;
    return variables;
  }

  // Authentification for user
  variables.push_back("AUTH_TYPE=");

  // Lenght of the request body, can be null or unset
  Result<std::__1::string> content_length = req.get_header("content-lenght");
  std::string content_length_str;
  if (content_length.is_ok()) {
    content_length_str = "CONTENT_LENGTH=" + content_length.unwrap();
    std::cout << "content lenght = " << content_length_str << std::endl;
    variables.push_back(content_length_str.c_str());
  }

  // Content type of the request body
  Result<std::__1::string> content_type = req.get_header("content-type");
  std::string content_type_str;
  if (content_type.is_ok()) {
    content_type_str = "CONTENT_TYPE=" + content_type.unwrap();
    variables.push_back(content_type_str.c_str());
  }

  // variables.push_back("PATH_TRANSLATED=/cgi_info.php");
  variables.push_back("PATH_TRANSLATED=/cgi_info.php");
  // Arguments for the script
  variables.push_back("QUERY_STRING=\"\"");
  // Client ip address version 4
  std::string client_ip = req.get_client_ip();
  client_ip = "REMOTE_ADDR=" + client_ip;
  variables.push_back(strdup(client_ip.c_str()));
  // ?
  variables.push_back("REMOTE_HOST=");
  // ?
  variables.push_back("REMOTE_IDENT=");
  // ?
  variables.push_back("REMOTE_USER=");
  // ?
  //----------------------------------------
  std::ostringstream ssn;
  ssn << "SERVER_NAME=" << serv.get_port();
  variables.push_back(ssn.str().c_str());
  variables.push_back("SERVER_NAME=");
  //----------------------------------------
  std::ostringstream ssp;
  ssp << "SERVER_PORT=" << serv.get_port();
  variables.push_back(ssp.str().c_str());
  //----------------------------------------
  // ?
  variables.push_back("SERVER_SOFTWARE=");
  // ?

  return variables;
}

void CGI::execute_cgi(std::string cgi_path, files::File const &file,
                      Request const &req, LocationConfig const &loc,
                      config::Server const &serv) {
  _cgi_path = cgi_path;
  _file_path = file.getPath();
  _status = cgi_status::ERROR;
  int pipes[2];

  char *env[20];
  int i = 0;
  std::vector<char const *> variables;
  variables = set_meta_variables(cgi_path, file, req, loc, serv);
  for (; i < variables.size();) {
    env[i] = strdup(variables[i]);
    std::cout << env[i] << std::endl;
    i++;
  }
  env[i] = NULL;

  char *cgi = strdup(_cgi_path.c_str());
  char *args[] = {cgi, NULL};

  if (pipe(pipes) < 0) {
    perror("pipes");
    _status = cgi_status::ERROR;
    return;
  }

  _child_pid = fork();
  if (_child_pid < 0) {
    perror("fork");
    _status = cgi_status::ERROR;
    return;
  }
  if (_child_pid == 0) {
    close(pipes[0]);
    dup2(pipes[1], 1);
    close(pipes[1]);
    execve(args[0], args, env);
    exit(1);
  } else {
    waitpid(_child_pid, &_child_return, WNOHANG);
    close(pipes[1]);
    _pipe = pipes[0];
    _status = cgi_status::READABLE;
  }
}
