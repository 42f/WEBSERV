#pragma once

#include <vector>
#include <istream>
#include <iostream>
#include <sstream>
#include "HTTP/Request/RequestLine.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "Config/Directives/Redirect.hpp"

#include "../utils/Logger.hpp"
#include "../Request.hpp"
#include "../Status.hpp"

#define BUFF_SIZE 512

class Response	{

	public:

		Response( void );
		Response( Version version, status::StatusCode statusCode );
		~Response( void );

		void	setVersion( const Version& version );
		void	setHeader( const Header& header );
		void	setStatus( const status::StatusCode& statusCode );

		void	reset( void );

		void	clearBody( void );
		std::vector<char> const&	getBody( void ) const;

		int				getBodyLen( void ) const;
		std::string&	getBodyLenStr( void );
		void			update_BodyLen( void );

	private:

		Version					_version;
		status::StatusCode		_statusCode;
		std::string			 	_statusMessage;

		std::map<std::string, Header>	_headers;
		std::vector<char>				_body;
		size_t							_bodyLength;
		std::string						_bodyLengthStr;
		size_t							_length;

		Response( Response const & src );
		Response &		operator=( Response const & rhs );

		friend std::ostream&	operator<<( std::ostringstream & o, Response const & i );
		friend std::istream&	operator>>( std::istream & is, Response& i );

};
