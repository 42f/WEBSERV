#include "Response.hpp"

/* ............................... CONSTRUCTOR ...............................*/

Response::Response( Request &req ) {
	(void)req;
}

Response::Response( Version version, status::StatusCode statusCode ) : _version (version),
														 _statusCode(statusCode) {

		 _statusMessage = status::StatusMessage::get(statusCode);
}


/* ..............................COPY CONSTRUCTOR.............................*/

/*
Response::Response( const Response & src )	{
}
*/

/* ................................ DESTRUCTOR ...............................*/

Response::~Response( void )	{
}

/* ................................. METHODS .................................*/

void	Response::set_header(const Header& header)
{
	this->_headers.insert(std::make_pair(header.name(), header));
}

/* ................................. ACCESSOR ................................*/



/* ................................. OVERLOAD ................................*/

/*
Response &				Response::operator=( Response const & rhs )	{
	if ( this != &rhs )	{
		this->_value = rhs.getValue();
	}
	return *this;
}
*/

std::ostream &			operator<<( std::ostream & o, Response const & i )	{
	o << "HTTP/" << i._version << " " << i._statusCode << " " << i._statusMessage << "\r\n";
	if (i._headers.empty() == false)	{
		for (std::map<std::string, Header>::const_iterator it = i._headers.begin();
													it != i._headers.end(); it++)	{
			o << it->second << "\r\n";
		}
	}
	return o;
}

/* ................................... DEBUG .................................*/


/* ................................. END CLASS................................*/
