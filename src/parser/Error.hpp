//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_ERROR_HPP
#define WEBSERV_ERROR_HPP

#include <vector>
#include <string>
#include <iostream>
#include "Constants.hpp"
#include "slice.hpp"
#include "utils.hpp"

template<typename Data = Empty>
class Error
{
private:
	Data										_data;
	std::string 								_msg;
	std::vector<std::pair<slice, std::string> >	_stack;

	typedef std::vector<std::pair<slice, std::string> >::const_iterator iterator;

public:

	Error(): _msg("No info") { }
	explicit Error(std::string msg): _msg(msg), _stack() { }
	explicit Error(std::string msg, Data data): _data(data), _msg(msg), _stack() { }

	Error(const Error& other)
	{
		if (&other == this)
			return ;
		this->_msg = other._msg;
		this->_stack = other._stack;
	}

	Data	content() const {
		return this->_data;
	}

	/*
	 * Insert a new element in the stacktrace
	 */
	Error		at(const slice &where, std::string info)
	{
		this->_stack.push_back(std::make_pair(where, info));
		return *this;
	}

	friend std::ostream	&operator<<(std::ostream &stream, const Error &err)
	{
		stream << YELLOW << err._msg << NC << std::endl;
		return stream;
	}

	void 		trace(slice start) const
	{
		for (Error::iterator it = _stack.begin(); it != _stack.end(); it++) {
			std::cerr << RED << "\tat: (l" << it->first.lines(start) << "c" << it->first.character(start) << "): " << it->first.take(10).until('\n') << " (" << it->second << ")" << NC << std::endl;
		}
	}
};

Error<Empty>	error(std::string msg);

#endif //WEBSERV_ERROR_HPP
