//
// Created by alena on 30/07/2021.
//

#include "Status.hpp"

namespace status
{

	StatusMessage	StatusMessage::_instance;

	StatusMessage::StatusMessage()
	{
		_msgs[Ok] = "OK";
		_msgs[status::BadRequest] = "Bad Request";
		_msgs[status::NotImplemented] = "Not Implemented";
		_msgs[status::UnsupportedVersion] = "HTTP Version Not Supported";
	}

	std::string & StatusMessage::get(StatusCode status)
	{
		return StatusMessage::_instance._msgs[status];
	}

	std::string &message(StatusCode status)
	{
		return StatusMessage::get(status);
	}

}