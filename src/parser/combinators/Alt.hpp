//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_ALT_HPP
#define WEBSERV_ALT_HPP

#include "Parser.hpp"

/*
 * Takes up to eight parsers and returns the result of the first successful one
 * Eg: Alt(Tag("Hello"), tag("pizza")) ("Hello World")
 *  => result: Ok("Hello", left :" World")
 */
template<typename T1, typename T2, typename T3 = Failure<typename T1::data_type>,
		typename T4 = Failure<typename T1::data_type>, typename T5 = Failure<typename T1::data_type>,
		typename T6 = Failure<typename T1::data_type>, typename T7 = Failure<typename T1::data_type>,
		typename T8 = Failure<typename T1::data_type> >
class Alt: public Parser<typename T1::data_type>
{
private:
	bool	_failure;
	T1		_p1;
	T2		_p2;
	T3		_p3;
	T4		_p4;
	T5		_p5;
	T6		_p6;
	T7		_p7;
	T8		_p8;

public:
	typedef typename T1::data_type	data_type;
	typedef ParserResult<data_type>		result_type;

	Alt(T1 p1, T2 p2, T3 p3 = Failure<data_type>(), T4 p4 = Failure<data_type>(), T5 p5 = Failure<data_type>(),
		T6 p6 = Failure<data_type>(), T7 p7 = Failure<data_type>(), T8 p8 = Failure<data_type>()) :
		_failure(false), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8) { }

	result_type		operator()(const slice& input)
	{
		result_type	res = _p1(input);
		result_type best = res;
		if (res.is_ok() || (_failure && res.is_failure()))
			return res;
		res = _p2(input);
		if (res.is_ok() || (_failure && res.is_failure()))
			return res;
		if (res.left().size < best.left().size)
			best = res;
		res = _p3(input);
		if (res.is_ok() || (_failure && res.is_failure()))
			return res;
		if (res.left().size < best.left().size)
			best = res;
		res = _p4(input);
		if (res.is_ok() || (_failure && res.is_failure()))
			return res;
		if (res.left().size < best.left().size)
			best = res;
		res = _p5(input);
		if (res.is_ok() || (_failure && res.is_failure()))
			return res;
		if (res.left().size < best.left().size)
			best = res;
		res = _p6(input);
		if (res.is_ok() || (_failure && res.is_failure()))
			return res;
		if (res.left().size < best.left().size)
			best = res;
		res = _p7(input);
		if (res.is_ok() || (_failure && res.is_failure()))
			return res;
		if (res.left().size < best.left().size)
			best = res;
		res = _p8(input);
		if (res.is_ok() || (_failure && res.is_failure()))
			return res;
		return best.unwind(input, "Alt: no match");
	}

	Alt		no_failure()
	{
		this->_failure = true;
		return *this;
	}
};

template<typename T1, typename T2>
Alt<T1, T2, Failure<typename T1::data_type>, Failure<typename T1::data_type>, Failure<typename T1::data_type>,
        Failure<typename T1::data_type>, Failure<typename T1::data_type>, Failure<typename T1::data_type> >
	alt(T1 p1, T2 p2)
{
	Failure<typename T1::data_type>	p = Failure<typename T1::data_type>();
	return Alt<T1, T2, Failure<typename T1::data_type>, Failure<typename T1::data_type>, Failure<typename T1::data_type>,
	        Failure<typename T1::data_type>, Failure<typename T1::data_type>, Failure<typename T1::data_type> >(
	        p1, p2, p, p, p, p, p, p);
}

template<typename T1, typename T2, typename T3>
Alt<T1, T2, T3, Failure<typename T1::data_type>, Failure<typename T1::data_type>,
        Failure<typename T1::data_type>, Failure<typename T1::data_type>, Failure<typename T1::data_type> >
	alt(T1 p1, T2 p2, T3 p3)
{
	Failure<typename T1::data_type>	p = Failure<typename T1::data_type>();
	return Alt<T1, T2, T3, Failure<typename T1::data_type>, Failure<typename T1::data_type>,
	        Failure<typename T1::data_type>, Failure<typename T1::data_type>, Failure<typename T1::data_type> >(
	        p1, p2, p3, p, p, p, p, p);
}

template<typename T1, typename T2, typename T3, typename T4>
Alt<T1, T2, T3, T4, Failure<typename T1::data_type>, Failure<typename T1::data_type>,
	Failure<typename T1::data_type>, Failure<typename T1::data_type> >	alt(T1 p1, T2 p2, T3 p3, T4 p4)
{
	Failure<typename T1::data_type>	p = Failure<typename T1::data_type>();
	return Alt<T1, T2, T3, T4, Failure<typename T1::data_type>,
	        Failure<typename T1::data_type>, Failure<typename T1::data_type>, Failure<typename T1::data_type> >(
	        p1, p2, p3, p4, p, p, p, p);
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
Alt<T1, T2, T3, T4, T5, Failure<typename T1::data_type>, Failure<typename T1::data_type>, Failure<typename T1::data_type> >
	alt(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5)
{
	Failure<typename T1::data_type> failure;
	return Alt<T1, T2, T3, T4, T5, Failure<typename T1::data_type>, Failure<typename T1::data_type>,
		Failure<typename T1::data_type> >(p1, p2, p3, p4, p5, failure, failure, failure);
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
Alt<T1, T2, T3, T4, T5, T6, Failure<typename T1::data_type>, Failure<typename T1::data_type> >
	alt(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6)
{
	Failure<typename T1::data_type> failure;
	return Alt<T1, T2, T3, T4, T5, T6, Failure<typename T1::data_type>, Failure<typename T1::data_type> >(p1, p2, p3, p4, p5, p6, failure, failure);
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
Alt<T1, T2, T3, T4, T5, T6, T7, Failure<typename T1::data_type> >	alt(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7)
{
	return Alt<T1, T2, T3, T4, T5, T6, T7, Failure<typename T1::data_type> >(p1, p2, p3, p4, p5, p6, p7, Failure<typename T1::data_type>());
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
Alt<T1, T2, T3, T4, T5, T6, T7, T8>	alt(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8)
{
	return Alt<T1, T2, T3, T4, T5, T6, T7, T8>(p1, p2, p3, p4, p5, p6, p7, p8);
}

#endif //WEBSERV_ALT_HPP
