//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_PARSER_HPP
#define WEBSERV_PARSER_HPP

#include "Result.hpp"
#include "slice.hpp"
#include "Error.hpp"

template<typename T>
class Parser
{
public:
	typedef T				data_type;
	typedef ParserResult<T>	result_type;

	virtual result_type		operator()(const slice& input) = 0;

};

template<typename T>
class Failure: public Parser<T>
{
public:
	Failure() { }
	typename Failure::result_type	operator()(const slice& input)
	{
		return ParserResult<T>::err(input, Error<status::StatusCode>());
	}
};

template<typename T>
class Success: public Parser<T>
{
public:
	Success() { }
	typename Success::result_type	operator()(const slice& input)
	{
		return ParserResult<T>::ok(input, T());
	}
};

#endif //WEBSERV_PARSER_HPP
