#pragma once

#include <vector>
#include <istream>
#include <iostream>
#include <sstream>
#include "HTTP/Request/RequestLine.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "Config/Directives/Redirect.hpp"

#include "../utils/Logger.hpp"
#include "HTTP/Request/Request.hpp"
#include "../Status.hpp"

class Response	{

	public:

		Response( void );
		Response( Response const & src );
		Response( Version version, status::StatusCode statusCode );

		Response &		operator=( Response const & rhs );
		~Response( void );

		void	setVersion( const Version& version );
		void	setHeader( const Header& header );
		void	setStatus( const status::StatusCode& statusCode );

		void	reset( void );

		void	clearBody( void );
		std::vector<char> const&	getBody( void ) const;

		int				getBodyLen( void ) const;
		std::string		getBodyLen( void );

		// std::string const&	getBodyLenStr( void ) const;
		void				update_BodyLen( void );

		std::string			 	_statusMessage;
	private:

		Version					_version;
		status::StatusCode		_statusCode;

		std::map<std::string, Header>	_headers;
		std::vector<char>				_body;
		size_t							_bodyLength;


		friend std::ostream&	operator<<( std::ostringstream & o, Response const & i );
		friend std::istream&	operator>>( std::istream & is, Response& i );

};
