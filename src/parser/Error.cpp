//
// Created by alena on 20/06/2021.
//

#include "Error.hpp"

Error<Empty>	error(std::string msg)
{
	return Error<Empty>(msg);
}
