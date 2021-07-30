//
// Created by alena on 15/07/2021.
//

#include "Methods.hpp"

namespace methods
{
	s_method	as_other(slice input)
	{
		(void)input;
		return methods::OTHER;
	}

	s_method	as_get(slice input)
	{
		(void)input;
		return methods::GET;
	}

	s_method	as_post(slice input)
	{
		(void)input;
		return methods::POST;
	}

	s_method	as_delete(slice input)
	{
		(void)input;
		return methods::DELETE;
	}
}

