//
// Created by alena on 14/07/2021.
//

#include "ServerName.hpp"

/*
 * ServerName = server_name RWS print
 */

ServerName::ServerName() { }

ServerName::result_type ServerName::operator()(const slice &input)
{
	return preceded(sequence(Tag("server_name"), rws), TakeUntil(";"))(input);
}