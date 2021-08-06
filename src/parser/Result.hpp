//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_RESULT_HPP
#define WEBSERV_RESULT_HPP

#include "Error.hpp"
#include "HTTP/Status.hpp"
#include "slice.hpp"

class DefaultError
{
private:
	std::string 	_what;

public:
	DefaultError(std::string err): _what(err) { }

	std::string 	what() { return _what; }
};

template<typename Ok, typename Err = DefaultError >
class Result
{
private:
	Ok		*_data;
	Err		*_err;
	bool	_is_ok;

	Result() : _err(0), _data(0), _is_ok(false) { }

public:

	~Result()
	{
		if (this->is_ok())
			delete _data;
		else
			delete _err;
	}

	Result(Ok d): _data(new Ok(d)), _err(0), _is_ok(true) { }
	Result(Err e): _data(0), _err(new Err(e)), _is_ok(false) { }

	Result(const Result& other): _data(0), _err(0), _is_ok(other.is_ok())
	{
		if (_is_ok)
			_data = new Ok(*other._data);
		else
			_err = new Err(*other._err);
	}

	Result	operator=(const Result& other)
	{
		if (this == &other)
			return *this;
		this->~Result();
		_data = 0;
		_err = 0;
		_is_ok = other._is_ok;
		if (_is_ok)
			_data = new Ok(*other._data);
		else
			_err = new Err(*other._err);
		return *this;
	}

	static Result ok(Ok data) { return Result(data); }
	static Result err(Err err) { return Result(err); }

	bool	is_ok() const { return this->_is_ok; }
	bool	is_err() const { return !is_ok(); }

	/*
	 * unwrap the result or use the provided default
	 */
	Ok	unwrap_or(Ok other) {
		if (_is_ok)
			return this->unwrap();
		return other;
	}
	/*
	 * Dont access it before checking if the result is an ok (is_ok() == true)
	 */
	Ok	&unwrap() const { return *this->_data; }
	Ok	&unwrap() { return *this->_data; }

	/*
	 * Dont access it before checking if the result is an err (is_err() == true)
	 */
	Err	&unwrap_err() const { return *this->_err; }
	Err &unwrap_err() { return *this->_err; }

	/*
	 * Converts this result to another Data type by using the supplied function
	 * if the result is an error, casts it to the correct type
	 */
	template<typename Output>
	Result<Output>	map(Output (*fn)(Result<Ok>&))
	{
		if (this->is_ok())
			return Result<Output>::ok(fn(*this));
		return this->convert<Output>();
	}

	template<typename Output>
	Result<Output>	map_to(Output data)
	{
		if (this->is_ok())
			return Result<Output>::ok(data);
		return this->convert<Output>();
	}

	/*
	 * Casts this error to another Result<type>
	 */
	template<typename Output>
	Result<Output>	convert()
	{
		return Result<Output>::err(*this->_err);
	}
};

template<typename Data, typename E = status::StatusCode>
class ParserResult {
private:
	typedef std::pair<slice, Data> data_type;
	typedef std::pair<slice, Error<E> > error_type;
	typedef Result<data_type, error_type> result_type;

	result_type _res;
	bool _failure;

	ParserResult() : _res(), _failure(false) {}

public:
	~ParserResult() {}

	ParserResult(const slice &left, Data d) : _res(result_type(std::make_pair(left, d))), _failure(false) {}

	ParserResult(const slice &left, Error<E> e, bool failure = false) : _res(result_type(std::make_pair(left, e))),
																		_failure(failure) {}

	ParserResult(const ParserResult &other) : _res(other._res), _failure(other._failure) {}

	static ParserResult ok(const slice &left, Data data) { return ParserResult(left, data); }

	static ParserResult err(const slice &left, Error<E> err) { return ParserResult(left, err); }

	static ParserResult fail(const slice &left, Error<E> err) { return ParserResult(left, err, true); }

	bool is_ok() const { return this->_res.is_ok(); }

	bool is_err() const { return this->_res.is_err(); }

	slice left() {
		if (_res.is_ok())
			return _res.unwrap().first;
		else
			return _res.unwrap_err().first;
	}

	/*
	 * Dont access it before checking if the result is an ok (is_ok() == true)
	 */
	Data &unwrap() const { return this->_res.unwrap().second; }

	Data &unwrap() { return this->_res.unwrap().second; }

	/*
	 * Dont access it before checking if the result is an err (is_err() == true)
	 */
	Error<E> &unwrap_err() const { return this->_res.unwrap_err().second; }

	Error<E> &unwrap_err() { return this->_res.unwrap_err().second; }

	/*
	 * Unwraps the error side and append a new item to the stacktrace
	 */
	ParserResult unwind(const slice &at, std::string msg) {
		this->unwrap_err().at(at, msg);
		return *this;
	}

	ParserResult failure() {
		if (this->is_err())
			this->_failure = true;
		return *this;
	}

	bool is_failure() const {
		return this->_failure;
	}


	/*
	 * Converts this result to another Data type by using the supplied function
	 * if the result is an error, casts it to the correct type
	 */
	template<typename Output>
	ParserResult<Output>	map(const slice &input, Output (*fn)(const slice &, ParserResult<Data> &)) {
		if (this->is_ok())
			return ParserResult<Output>::ok(this->left(), fn(input, *this));
		return this->convert<Output>();
	}

	template<typename Output>
	ParserResult<Output>	map(Output data) {
		if (this->is_ok())
			return ParserResult<Output>::ok(this->left(), data);
		return this->convert<Output>();
	}

	ParserResult		map_err(E value)
	{
		if (this->is_err())
			this->unwrap_err().replace(value);
		return *this;
	}

	template<typename Output>
	ParserResult<Output>	convert()
	{
		return ParserResult<Output>(
			_res.unwrap_err().first,
			_res.unwrap_err().second,
			_failure);
	}

};

#endif //WEBSERV_RESULT_HPP
