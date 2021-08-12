#include "Response.hpp"

/* ............................... CONSTRUCTOR ...............................*/

Response::Response() : _version(Version('1', '1')), _statusCode(status::None) {
}

Response::Response(Version version, status::StatusCode statusCode)
    : _version(version) {
    setStatus(statusCode);
}

/* ..............................COPY CONSTRUCTOR.............................*/

Response::Response(const Response& src) {
    if (this != &src) *this = src;
}

/* ................................ DESTRUCTOR ...............................*/

Response::~Response() {}

/* ................................. METHODS .................................*/

void Response::reset( Version const & vers, status::StatusCode code ) {
    _version = vers;
    _statusCode = code;
    _statusMessage = status::StatusMessage::get(_statusCode);
    _headers.clear();
}

void Response::setVersion(const Version& version) { _version = version; }

void Response::setHeader( std::string const& field, std::string const& value ) {
    _headers.insert(std::make_pair(field, std::make_pair(field, value)) );
    // _headers.insert(std::make_pair(field, value));
}

void Response::setHeader( std::string const& field, int value ) {
	std::stringstream strValue;
	strValue << value;
    setHeader(field, strValue.str());
}

void Response::setStatus(const status::StatusCode& statusCode) {
    _statusCode = statusCode;
    _statusMessage = status::StatusMessage::get(statusCode);
}

files::File &   Response::getFile( void ) { return _file; }


/* ................................. ACCESSOR ................................*/


/* ................................. OVERLOAD ................................*/

Response& Response::operator=(Response const& rhs) {
    if (this != &rhs) {
        this->_version = rhs._version;
        this->_statusCode = rhs._statusCode;
        this->_statusMessage = rhs._statusMessage;
        this->_headers = rhs._headers;
        this->_file = rhs._file;
    }
    return *this;
}

// Writes the response's content to the client's connection fd
std::ostream& operator<<(std::ostringstream& o, Response const& i) {
    // Writes status line
    o << "HTTP/" << i._version << " " << i._statusCode << " "
      << i._statusMessage << "\r\n";

    // Writes headers
    if (i._headers.empty() == false) {

        Response::headerMap_t::const_iterator it = i._headers.begin();
        for (; it != i._headers.end(); it++) {
            o << it->first << ": " << it->second.second << "\r\n";
        }
    }

    // TODO remove or find a cheap way to cut short the body.
    LogStream stream;
    stream << "Response sent:" << o.str();

    // Writes empty line separation
    o << "\r\n";

    return o;
}

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
