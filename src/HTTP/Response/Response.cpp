#include "Response.hpp"

/* ............................... CONSTRUCTOR ...............................*/

Response::Response()
    : _respState(respState::emptyResp),
      _version(Version('1', '1')),
      _statusCode(status::None) {
  setHeader("Cache-Control:", "no-cache");  // TODO remove debug
  setHeader(headerTitle::Date, Timer::getTimeNow());
  setHeader(headerTitle::Server,
            headerTitle::DefaultValues::get(headerTitle::Server));
}

Response::Response(Version version, status::StatusCode statusCode)
    : _respState(respState::emptyResp),
      _version(version),
      _statusCode(statusCode) {
  setHeader(headerTitle::Date, Timer::getTimeNow());
  setHeader(headerTitle::Server,
            headerTitle::DefaultValues::get(headerTitle::Server));
}

/* ..............................COPY CONSTRUCTOR.............................*/

Response::Response(const Response& src) {
  if (this != &src) *this = src;
}

/* ................................ DESTRUCTOR ...............................*/

Response::~Response() {}

/* ................................. METHODS .................................*/

void Response::reset(Version const& vers, status::StatusCode code) {
  _respState = respState::emptyResp;
  _version = vers;
  _statusCode = code;
  _headers.clear();
}

void Response::setVersion(const Version& version) { _version = version; }
void Response::setStatus(status::StatusCode code) { _statusCode = code; }

void Response::setHeader(std::string const& field, std::string const& value) {
  _headers.insert(std::make_pair(field, std::make_pair(field, value)));
}

void Response::setHeader(headerTitle::Title title, std::string const& value) {
  std::string titleStr = headerTitle::HeaderTitleField::get(title);
  _headers.insert(std::make_pair(titleStr, std::make_pair(titleStr, value)));
}

void Response::setHeader(std::string const& field, int value) {
  std::stringstream strValue;
  strValue << value;
  setHeader(field, strValue.str());
}

void Response::setHeader(headerTitle::Title title, int value) {
  std::string titleStr = headerTitle::HeaderTitleField::get(title);
  std::stringstream strValue;
  strValue << value;
  setHeader(titleStr, strValue.str());
}

void Response::setFile(std::string const& filePath) {
  if (filePath.empty() == false) _file = files::File(filePath);
}

CGI& Response::getCgiInst(void) { return _cgi; }
files::File const& Response::getFileInst(void) const { return _file; }
int Response::getFileFD(void) const { return _file.getFD(); }
status::StatusCode Response::getStatusCode(void) const { return _statusCode; }
std::string& Response::getErrorBuffer(void) { return _htmlErrorBuffer; }
int& Response::getState(void) { return _respState; }

void Response::loadErrorHtmlBuffer(const status::StatusCode& code,
                                   const std::string& optionalMessage) {
  std::stringstream tmpBuff;

  tmpBuff << "<html>" << '\n'
          << "<head>" << '\n'
          << "	<title>" << code << ' ' << status::StatusMessage::get(code)
          << "</title>" << '\n'
          << "</head>" << '\n'
          << "<body>" << '\n'
          << "	<center>" << '\n'
          << "		<h1> Ho crap ! That's an error. </h1>" << '\n'
          << "		<h1> " << code << ' ' << status::StatusMessage::get(code)
          << " </h1>" << '\n';
  if (optionalMessage.empty() == false)
    tmpBuff << "		<h1> " << optionalMessage << " </h1>" << '\n';
  tmpBuff << "	</center>" << '\n'
          << "	<hr>" << '\n'
          << "	<center>Webserv Team ABC</center>" << '\n'
          << "</body>" << '\n'
          << "</html>" << '\n';
  _htmlErrorBuffer.assign(tmpBuff.str());
}

/* ................................. ACCESSOR ................................*/

/* ................................. OVERLOAD ................................*/

Response& Response::operator=(Response const& rhs) {
  if (this != &rhs) {
    this->_respState = rhs._respState;
    this->_version = rhs._version;
    this->_statusCode = rhs._statusCode;
    this->_headers = rhs._headers;
    this->_file = rhs._file;
  }
  return *this;
}

// Writes the response's content to the client's connection fd
std::ostream& operator<<(std::ostream& o, Response const& i) {
  // Writes status line
  o << "HTTP/" << i._version << " " << i._statusCode << " "
    << status::StatusMessage::get(i._statusCode) << "\r\n";

  // Writes headers
  if (i._headers.empty() == false) {
    Response::headerMap_t::const_iterator it = i._headers.begin();
    for (; it != i._headers.end(); it++) {
      o << it->first << ": " << it->second.second << "\r\n";
    }
  }

  // Writes empty line separation
  // o << "\r\n";

  return o;
}

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
