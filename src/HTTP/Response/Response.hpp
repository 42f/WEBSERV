#pragma once

#include <vector>
#include <istream>
#include <iostream>
#include <sstream>

#include "Config/Directives/Redirect.hpp"
#include "RequestLine.hpp"
#include "Headers.hpp"
#include "ResponseHeader.hpp"
#include "File.hpp"

#include "utils/Logger.hpp"
#include "Status.hpp"

class Response	{

	public:

		Response( void );
		Response( Response const & src );
		Response( Version version, status::StatusCode statusCode );

		Response &		operator=( Response const & rhs );
		~Response( void );

		void	setVersion( const Version& version );
		void	setHeader( std::string const& field, std::string const& value );
		void	setHeader( std::string const& field, int value );
		void	setStatus( const status::StatusCode& statusCode );

		files::File & 	getFile( void );

		void	reset( Version const & vers = Version(), status::StatusCode code = status::None );

		std::string			 	_statusMessage;
	private:

		typedef	std::pair<std::string const, std::string>		header_t;
		typedef	std::map<std::string const, header_t>			headerMap_t;


		Version										_version;
		status::StatusCode							_statusCode;
		headerMap_t									_headers;
		files::File									_file;

		friend std::ostream&	operator<<( std::ostringstream & o, Response const & i );
};
