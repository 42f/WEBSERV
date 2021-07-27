//
// Created by alena on 20/06/2021.
//

#ifndef WEBSERV_SCHEME_HPP
#define WEBSERV_SCHEME_HPP

#include "export.hpp"

/*
 * scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
 */
static int scheme_charset(int c);

class Scheme: public Parser<slice>
{
public:
	Scheme();

	result_type 	operator()(const slice &input);
};

#endif //WEBSERV_SCHEME_HPP
