//
// Created by alena on 15/07/2021.
//

#include "Scheme.hpp"

int scheme_charset(int c) { return isalnum(c) || c == '+' || c == '-' || c == '.'; }

/*
 * scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
 */
Scheme::Scheme(){ }
Scheme::result_type	Scheme::operator()(const slice &input)
{
	return as_slice(sequence(alpha, TakeWhile(scheme_charset, true)))(input);
}

//TODO REMOVE FROM ?