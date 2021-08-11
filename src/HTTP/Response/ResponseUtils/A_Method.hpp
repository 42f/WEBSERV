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

		std::cout << BLUE << "LOCATION USED: " << loc << NC <<std::endl;
		std::string	targetFile(loc.get_root() + "/");
		targetFile += req.target.isFile() ? req.target.getFile() : loc.get_index();

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
	}
};
