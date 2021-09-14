#pragma once

#include <vector>
#include <istream>
#include <iostream>
#include <sstream>

#include "Config/Directives/Redirect.hpp"
#include "Config/Server.hpp"
#include "HTTP/Request/RequestUtils/RequestLine.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "ResponseUtils/ResponseHeader.hpp"
#include "ResponseUtils/File.hpp"

#include "utils/Logger.hpp"
#include "utils/Timer.hpp"
#include "HTTP/Status.hpp"
#include "CGI/CGI.hpp"

namespace respState {

	// bit mask on a int
	enum states {
		emptyResp		= 0,
		entirelySent	= 1<<0,
		ioError 		= 1<<1,
		headerSent		= 1<<2,
		buffResp 		= 1<<3,
		cgiResp 		= 1<<4,
		fileResp 		= 1<<5,
		chunkedResp		= 1<<6,
		noBodyResp		= 1<<7,
		cgiHeadersSent	= 1<<8
	};
}

class Response	{

	public:

		Response( void );
		Response( Version version, status::StatusCode statusCode );

		~Response( void );

		void	setVersion( const Version& version );
		void	setStatus( status::StatusCode code );

		void	setHeader( std::string const& field, std::string value );
		void	setHeader( headerTitle::Title title, std::string value );
		void	setHeader( std::string const& field, int value );
		void	setHeader( headerTitle::Title title, int value );

		void	loadErrorHtmlBuffer( const status::StatusCode& code,
									const std::string& optionalMessage = "" );

		files::File &			setFile( std::string const & filePath );
		files::File &			setUploadFile( std::string const & filePath );

		CGI  &			 		getCgiInst( void ) ;
		CGI const &		 		getCgiInst( void ) const;
		int						getCgiFD( void ) const;

		void					setUploadFd( int fd );
		int						getUploadFd( void ) const;
		files::File const & 	getFileInst( void ) const;
		int						getFileFD( void ) const;
		status::StatusCode		getStatusCode( void ) const;
		std::string &			getBuffer( void );
		int &					getState( void );

		void	reset( Version const & vers = Version(), status::StatusCode code = status::None );

	private:

		Response( Response const & src );
		typedef	std::pair<std::string const, std::string>		header_t;
		typedef	std::map<std::string const, header_t>			headerMap_t;


		int											_respState;
		Version										_version;
		status::StatusCode							_statusCode;
		headerMap_t									_headers;

		// body data
		files::File									_file;
		files::File									_uploadFile;

		CGI											_cgi;
		std::string									_htmlBuffer;
		int											_uploadFd;

		friend std::ostream&	operator<<( std::ostream & o, Response const & i );
};
