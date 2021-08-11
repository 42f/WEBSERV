//
// Created by alena on 14/07/2021.
//

#include "Upload.hpp"

/*
 * Upload = upload on | off
 */
Upload::Upload() { }

Upload::result_type Upload::operator()(const slice &input)
{
	ParserResult<slice> res = preceded(sequence(Tag("upload"), rws), alt(Tag("on"), Tag("off")))(input);
	if (res.is_ok())
		return res.map(res.unwrap().ieq("on"));
	return res.convert<bool>();
}

/* ************************************************************************** */
