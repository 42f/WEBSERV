#pragma once

#include "HTTP/Response/Response.hpp"
#include "HTTP/Response/ResponseHandler.hpp"
#include "HTTP/Status.hpp"

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
		if (req.target.isFile()) {
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
		files::File f(targetFile);
		if (f.isGood()) {
			f.getStream() >> resp;
			resp.setHeader(ResponseHeader(headerTitle::Content_Length, resp.getBodyLen()));
			resp.setHeader(ResponseHeader(headerTitle::Content_Type, "text/html; charset=UTF-8"));
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
