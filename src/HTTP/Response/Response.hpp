#pragma once

#include "HTTP/Request/RequestLine.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "Config/Directives/Redirect.hpp"
#include "../Request.hpp"
#include "../Status.hpp"

class Response	{

	public:

		Response( Request &req );
		Response( Version version, status::StatusCode statusCode );

		void	set_header(const Header& header);
		~Response( void );

	private:

		Version					_version;
		status::StatusCode		_statusCode;
		std::string			 	_statusMessage;

		std::map<std::string, Header>	_headers;
		std::vector<char>				_body;
		size_t							_length;

		Response( Response const & src );
		Response &		operator=( Response const & rhs );

		friend std::ostream &	operator<<( std::ostream & o, Response const & i );

};

