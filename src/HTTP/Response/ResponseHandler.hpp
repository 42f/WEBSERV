#pragma once

# include <vector>
# include <istream>
# include <iostream>
# include <sstream>
# include <unistd.h>
# include <iostream>
# include <string>
# include <fstream>
# include <iterator>

#include "Request/RequestLine.hpp"
#include "Headers/Headers.hpp"
#include "Config/Directives/Redirect.hpp"

#include "utils/Logger.hpp"
#include "HTTP/Request/Request.hpp"
#include "RequestHandler.hpp"
#include "Status.hpp"
#include "Response.hpp"
#include "Config/Server.hpp"
#include "Timer.hpp"


namespace response_status
{
	enum Status
	{
		Empty,			// Not treated yet
		Waiting,		// Waiting on full body (large file or cgi)
		Ready		// Ready to be sent to client
	};
}

class ResponseHandler	{

		class A_Method;
		typedef	RequestHandler::result_type		ReqResult;

	public:

		void			init( ReqResult const & requestResult, int receivedPort );
		void	 		processRequest( void );

		bool		 	isReady( void );
		Response const&	getResponse( void );

		ResponseHandler( void );
		ResponseHandler( ReqResult requestResult, int receivedPort );
		~ResponseHandler( void );

	private:

		int		 							_port;
		ReqResult 							_request;
		response_status::Status				_status;
		Response							_response;
		A_Method *							_method;

		std::string			getHeader(const Request & req, const std::string& target);

		ResponseHandler( ResponseHandler const & src );
		ResponseHandler &		operator=( ResponseHandler const & rhs );


/*
 * GET POST and DELETE Methods
 */

		class A_Method	{

			public:
				A_Method() {};
				virtual ~A_Method() {};

				virtual void	handler(config::Server serv, LocationConfig loc, Request req, Response & resp) = 0;

				static Response		stdResponse(status::StatusCode code, LocationConfig const & loc) {

					Response output;
					output.setStatus(code);
					// TODO check for default error page in location
					(void)loc;
					// TODO if none, load body with default error page if any available
					return output;
				}
		};

		class GetMethod	: public A_Method {

			public:

			GetMethod() {};
			~GetMethod() {};

			void	handler(config::Server serv, LocationConfig loc, Request req, Response & resp) {

				(void)serv;

				// Case where no location was resolved, and parent server has no root
				if (loc.get_root().empty())	{
					resp = Response(Version('r', 'n'), status::Unauthorized); // TODO Version debug only
					return ;
				}
				std::string	targetFile(loc.get_root());
				if (files::File::isFile(req.target.decoded_path)) {
					if (req.target.decoded_path.find(loc.get_path()) == 0) {
						LogStream s; s << "old targetfile is : " << targetFile << "\n";
						targetFile += req.target.decoded_path.substr(loc.get_path().length()) ;
						s << "new targetfile is : " << targetFile  ;
					}
					else {
						targetFile += req.target.decoded_path ;
					}
				}
				else if (loc.get_index().empty() == false)	{
					targetFile += loc.get_index();
				}
				else {
					resp = Response(Version('1', '1'), status::NotFound);
					return ;
				}

				LogStream s; s << "FILE TARGETED PATH:" << targetFile;


				resp.setFile(targetFile);
				if (resp.getFile().isGood()) {

					resp.setHeader(headerTitle::HeaderTitleField::get(headerTitle::Content_Length), resp.getFile().getSize());
					resp.setHeader(headerTitle::HeaderTitleField::get(headerTitle::Content_Type), resp.getFile().getType());
					resp.setStatus(status::Ok);
				}
				else
					resp = Response(Version('2', '1'), status::NotFound);
			}
		};

		class PostMethod	: public A_Method {

			public:

			PostMethod() {};
			~PostMethod() {};

			void	handler(config::Server serv, LocationConfig loc, Request req, Response & resp) {
				(void)serv;
				(void)resp;
				(void)loc;
				(void)req;
				std::cout << __func__ << " of POST." << std::endl;
			}
		};

		class DeleteMethod	: public A_Method {

			public:

			DeleteMethod() {};
			~DeleteMethod() {};

			void	handler(config::Server serv, LocationConfig loc, Request req, Response & resp) {
				(void)serv;
				(void)resp;
				(void)loc;
				(void)req;
				std::cout << __func__ << " of DELETE." << std::endl;
			}
		};

		class UnsupportedMethod	: public A_Method {

			public:

			UnsupportedMethod() {};
			~UnsupportedMethod() {};

			void	handler(config::Server, LocationConfig, Request, Response&) {
				std::cout << __func__ << " of UNSUPPORTED." << std::endl;
			}
		};

}; // end reponseHandler
