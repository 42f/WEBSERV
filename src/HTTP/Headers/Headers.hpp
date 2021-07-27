//
// Created by alena on 24/06/2021.
//

#ifndef WEBSERV_HEADERS_HPP
#define WEBSERV_HEADERS_HPP

#include "parser/combinators.hpp"
#include "Header.hpp"

typedef TakeWhile		ContentLength;
const ContentLength		CONTENT_LENGTH = TakeWhile(std::isdigit);

class Headers: public Parser<Header> {

public:
	Headers() { }

	result_type 	operator()(const slice& input) {
		return alt(
			header("Content-Length", CONTENT_LENGTH),
			HeaderParser<>())(input);
	}
};

//TODO more headers... and cpp

#endif //WEBSERV_HEADERS_HPP
