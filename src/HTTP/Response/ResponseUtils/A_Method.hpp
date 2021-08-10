#pragma once

#include "HTTP/Response/Response.hpp"
#include "HTTP/Response/ResponseHandler.hpp"
#include "HTTP/Status.hpp"

class A_Method	{

	public:
		A_Method() {
			std::cout << __func__ << std::endl;
		};
		virtual ~A_Method() {};

		virtual Response	handler(config::Server serv, LocationConfig loc, Request req) = 0;

		Response	stdResponse(status::StatusCode code, LocationConfig const & loc) {

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

	Response	handler(config::Server serv, LocationConfig loc, Request req) {
		(void)serv;
		(void)loc;
		(void)req;
		std::cout << __func__ << " of GET." << std::endl;
		return Response(Version(), status::RequestTimeout);
	}
};

class PostMethod	: public A_Method {

	public:

	PostMethod() {};
	~PostMethod() {};

	Response	handler(config::Server serv, LocationConfig loc, Request req) {
		(void)serv;
		(void)loc;
		(void)req;
		std::cout << __func__ << " of POST." << std::endl;
		return Response(Version(), status::NotImplemented); // TODO REMOVE TEST RETURN
	}
};

class DeleteMethod	: public A_Method {

	public:

	DeleteMethod() {};
	~DeleteMethod() {};

	Response	handler(config::Server serv, LocationConfig loc, Request req) {
		(void)serv;
		(void)loc;
		(void)req;
		std::cout << __func__ << " of DELETE." << std::endl;
		return Response(Version(), status::PreconditionRequired); // TODO REMOVE TEST RETURN
	}
};

class UnsupportedMethod	: public A_Method {

	public:

	UnsupportedMethod() {};
	~UnsupportedMethod() {};

	Response	handler(config::Server, LocationConfig, Request) {
		return Response(Version(), status::NotImplemented); // TODO REMOVE TEST RETURN
	}
};
