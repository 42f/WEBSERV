#include "Response.hpp"

/* ............................... CONSTRUCTOR ...............................*/

Response::Response( void ) {
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

Response::~Response( void )	{
}

/* ................................. METHODS .................................*/

void	Response::reset( void )	{
	_version = Version();
	_statusCode = status::None;
	_statusMessage = status::StatusMessage::get(_statusCode);
	_headers.clear();
	clearBody();
 };

void	Response::clearBody( void )	{
	_body.clear();
	update_BodyLen();
};

std::vector<char> const &	Response::getBody( void ) const	{ return _body; };

int				Response::getBodyLen( void ) const	{ return _bodyLength; };
std::string&	Response::getBodyLenStr( void ) { return _bodyLengthStr; };


void	Response::update_BodyLen( void ) {

	_bodyLength = _body.size();

	std::stringstream	len;
	len << _bodyLength;
	_bodyLengthStr = len.str();
};

/* ................................. ACCESSOR ................................*/


//TODO is this useful ?...
void	Response::setVersion( const Version& version )	{ _version = version; };

void	Response::setHeader(const Header& header) {
	this->_headers.insert(std::make_pair(header.name(), header));
}

void	Response::setStatus( const status::StatusCode &statusCode )	{
	_statusCode = statusCode;
	_statusMessage = status::StatusMessage::get(statusCode);
};

/* ................................. OVERLOAD ................................*/

Response &				Response::operator=( Response const & rhs )	{
	if ( this != &rhs )	{
		this->_version = rhs._version;
		this->_statusCode = rhs._statusCode;
		this->_statusMessage = rhs._statusMessage;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_length = rhs._length;
	}
	return *this;
}

// TODO get stream of bytes in the response _body
std::istream &			operator>>( std::istream & is, Response& inst )	{
	char buff[BUFF_SIZE];

	while (is.good()) {
		bzero(&buff, BUFF_SIZE);
		is.getline(buff, BUFF_SIZE);
		inst._body.reserve(inst._body.size() + BUFF_SIZE);
		for (int i = 0; i < BUFF_SIZE && buff[i] != '\0'; i++)
			inst._body.push_back(buff[i]);
	}
	inst.update_BodyLen();
	return is;
}

// Writes the response's content to the client's connection fd
std::ostream &			operator<<( std::ostream & o, Response const & i )	{

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

	return o;
}

/* ................................... DEBUG .................................*/


/* ................................. END CLASS................................*/
