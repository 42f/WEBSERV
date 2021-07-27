//
// Created by alena on 15/07/2021.
//

#ifndef WEBSERV_REQUESTLINE_HPP
#define WEBSERV_REQUESTLINE_HPP

#include "parser/export.hpp"
#include "Tokens.hpp"
#include "Methods.hpp"

#include "HTTP/Request/RequestTarget/OriginForm.hpp"

#ifdef minor
#undef minor
#endif

#ifdef major
#undef major
#endif

/*
 * RequestMethod = [GET | POST | DELETE] SP
 */

class RequestMethod : public Parser<methods::s_method>
{
public:
	RequestMethod();

	result_type	operator()(const slice &input);
};

std::ostream &operator<<(std::ostream& st, const methods::s_method& method);

/*
 * RequestTarget = OriginForm (| absolute-form)
 */

class RequestTarget: public Parser<Target>
{
public:
	RequestTarget();
	result_type	operator()(const slice &input);
};

/*
 * Version
 */
struct Version
{
	char major;
	char minor;
	
	Version(): major(0), minor(0) { }
	Version(char maj, char min): major(maj), minor(min) { }
	static Version	from_tuple(tuple<char, char> data)
	{
		return Version(data.first, data.second);
	}

	friend std::ostream& operator<<(std::ostream &stream, const Version &version)
	{
		stream << version.major << "." << version.minor;
		return stream;
	}
};

/*
 * RequestVersion = HTTP-version = HTTP "/" DIGIT "." DIGIT
 */
class RequestVersion: public Parser<Version>
{
public:
	RequestVersion();
	result_type operator()(const slice &input);
};

/*
 * RequestLine = RequestMethod SP RequestTarget SP RequestVersion CRLF
 */
class RequestLine: public Parser<tuple<methods::s_method, Target, Version> >
{
public:
	RequestLine();
	result_type operator()(const slice &input);
};


#endif //WEBSERV_REQUESTLINE_HPP
