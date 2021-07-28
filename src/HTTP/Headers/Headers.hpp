//
// Created by alena on 24/06/2021.
//

#ifndef WEBSERV_HEADERS_HPP
#define WEBSERV_HEADERS_HPP

#include "export.hpp"
#include "Header.hpp"
#include "HeaderParser.hpp"

typedef TakeWhile		ContentLength;
const ContentLength		CONTENT_LENGTH = TakeWhile(std::isdigit);

class Headers: public Parser<Header>
{
public:
	Headers();

	result_type 	operator()(const slice& input);
};

//TODO more headers...

#endif //WEBSERV_HEADERS_HPP
