#pragma once

#include <vector>
#include <istream>
#include <iostream>
#include <sstream>
#include "HTTP/Request/RequestLine.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "HTTP/Response/ResponseUtils/File.hpp"
#include "HTTP/Response/ResponseUtils/ResponseHeader.hpp"
#include "Config/Directives/Redirect.hpp"

#include "utils/Logger.hpp"
#include "HTTP/Response/ResponseUtils/File.hpp"
#include "HTTP/Request/Request.hpp"
#include "Status.hpp"


class Response	{

	public:


		Response( void );
		Response( Response const & src );
		Response( Version version, status::StatusCode statusCode );

		Response &		operator=( Response const & rhs );
		~Response( void );

		void	setVersion( const Version& version );
		void	setHeader( ResponseHeader newHeader );
		void	setStatus( const status::StatusCode& statusCode );

		void	reset( void );

		void	clearBody( void );
		std::vector<char> const&	getBody( void ) const;

		int				getBodyLen( void ) const;
		std::string		getBodyLen( void );

		void			update_BodyLen( void );

		std::string			 	_statusMessage;
	private:

		Version					_version;
		status::StatusCode		_statusCode;

		std::map<std::string, ResponseHeader>	_headers;
		std::vector<char>				_body;
		size_t							_bodyLength;


		friend std::ostream&	operator<<( std::ostringstream & o, Response const & i );
		friend std::istream&	operator>>( std::istream & is, Response& i );
};
