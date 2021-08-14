//
// Created by alena on 27/07/2021.
//

#include "Eof.hpp"

Eof::Eof() { }

Eof::result_type Eof::operator()(const slice &input)
{
	if (*input.p == 0)
		return result_type::ok(input, slice());
	return result_type::err(input, error("Eof: not the end"));
}