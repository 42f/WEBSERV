//
// Created by alena on 20/06/2021.
//

#include "Error.hpp"

Error<status::StatusCode>	error(std::string msg, status::StatusCode status)
{
	return Error<status::StatusCode>(msg, status);
}


