#include "Response.hpp"

/* ............................... CONSTRUCTOR ...............................*/

Response::Response() : _version(Version()), _statusCode(status::None) {
}

Response::Response( Version version, status::StatusCode statusCode ) :
														_version (version) {
	setStatus(statusCode);
}


/* ..............................COPY CONSTRUCTOR.............................*/

Response::Response( const Response & src )	{
	if (this != &src)
		*this = src;
}

/* ................................ DESTRUCTOR ...............................*/

Response::~Response()	{
}

/* ................................. METHODS .................................*/

void	Response::reset()	{
	_version = Version();
	_statusCode = status::None;
	_statusMessage = status::StatusMessage::get(_statusCode);
	_headers.clear();
	clearBody();
 }

void	Response::clearBody()	{
	_body.clear();
	update_BodyLen();
}


void	Response::update_BodyLen()  {
	_bodyLength = _body.size();
	setHeader( Header(slice("Content-Length"), slice("42")) );

	setHeader(Header(_statusMessage, _statusMessage));

}

void	Response::setVersion( const Version& version )	{ _version = version; }

void	Response::setHeader(const Header& header) {
	_headers.insert(std::make_pair(header.name(), header));
}

void	Response::setStatus( const status::StatusCode &statusCode )	{
	_statusCode = statusCode;
	_statusMessage = status::StatusMessage::get(statusCode);
}

/* ................................. ACCESSOR ................................*/

std::vector<char> const &	Response::getBody() const	{ return _body; }

int				Response::getBodyLen() const	{ return _bodyLength; }

std::string 	Response::getBodyLen() {
	std::stringstream	len;
	len << _bodyLength;
	return len.str().c_str();
}

/* ................................. OVERLOAD ................................*/

Response &		Response::operator=( Response const & rhs )	{
	if ( this != &rhs )	{
		this->_version = rhs._version;
		this->_statusCode = rhs._statusCode;
		this->_statusMessage = rhs._statusMessage;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
	}
	return *this;
}

// appends the content of istream in the response _body, use clearBody() before if needed.
std::istream &	operator>>( std::istream & is, Response& inst )	{

	std::string		buffer;

	while (is.good()) {
		std::getline(is, buffer);
		if (is.eof() == false)
			buffer += '\n'; // add back the newline removed by getline as a delimiter
		inst._body.insert(inst._body.end(), buffer.begin(), buffer.end());
	}
	inst.update_BodyLen();
	return is;
}

// Writes the response's content to the client's connection fd
std::ostream &	operator<<( std::ostringstream & o, Response const & i )	{

	// Writes status line
	o << "HTTP/" << i._version << " " << i._statusCode << " " << i._statusMessage << "\r\n";

	// Writes headers
	if (i._headers.empty() == false)	{
		for (std::map<std::string, Header>::const_iterator it = i._headers.begin();
													it != i._headers.end(); it++)	{
			o << it->second << "\r\n";
		}
	}

	// Writes empty line separation
	o << "\r\n";

	// Writes body's content
	if (i._body.empty() == false) {
		o.write(i._body.data(), i._body.size());
	}

	// TODO remove or find a cheap way to cut short the body.
	Logger::log(std::string("Response sent: \n") + o.str(), Logger::toConsole);
	return o;
}

/* ................................... DEBUG .................................*/


/* ................................. END CLASS................................*/
