//
// Created by alena on 27/07/2021.
//

#ifndef WEBSERV_EOF_HPP
#define WEBSERV_EOF_HPP

#include "Parser.hpp"

// Matches the end of the slice (no input left)
class Eof: public Parser<slice>
{
public:
	Eof();
	result_type operator()(const slice &input);
};


#endif //WEBSERV_EOF_HPP
