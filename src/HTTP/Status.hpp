//
// Created by alena on 30/07/2021.
//

#ifndef WEBSERV_STATUS_HPP
#define WEBSERV_STATUS_HPP

#include <map>
#include <string>

namespace status
{
	enum StatusCode
	{
		None = 0,
		Ok = 200,
		BadRequest = 400,
		NotImplemented = 501,
		UnsupportedVersion = 505
	};

	class StatusMessage
	{
	private:
		std::map<StatusCode, std::string>	_msgs;
		static StatusMessage				_instance;

		StatusMessage();

	public:
		static std::string&	get(StatusCode status);
	};

	std::string&	message(StatusCode status);
}

#endif //WEBSERV_STATUS_HPP
