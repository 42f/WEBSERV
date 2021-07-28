//
// Created by alena on 28/07/2021.
//

#include "Headers.hpp"

Headers::Headers() { }

Headers::result_type		Headers::operator()(const slice& input)
{
	return alt(
			header("Content-Length", CONTENT_LENGTH),
			HeaderParser<>())(input);
}