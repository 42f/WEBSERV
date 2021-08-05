//
// Created by alena on 14/07/2021.
//

#include "Cgi.hpp"

/*
 * Cgi = cgi .extension Path
 */

Cgi::Cgi() { }

Cgi::result_type Cgi::operator()(const slice &input)
{
	return sequence(delimited(sequence(Tag("cgi"), rws),
				preceded(Tag("."), TakeUntil(" ")),
			rws),TakeUntil(";"))(input);
}

/* ************************************************************************** */
