//
// Created by alena on 10/07/2021.
//

#ifndef WEBSERV_UPLOAD_HPP
#define WEBSERV_UPLOAD_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"

/*
 * Upload = upload on | off
 */
class Upload : public Parser<bool>
{
public :
	Upload();

	result_type operator()(const slice &input);
};

#endif //WEBSERV_UPLOAD_HPP
