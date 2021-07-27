//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_SEQUENCE_HPP
#define WEBSERV_SEQUENCE_HPP

#include "Parser.hpp"
#include "Error.hpp"

/*
 * Takes up to 8 parsers and parse them in sequence
 * Eg: Sequence(Tag("Hello"), Tag("World"))("HelloWorld")
 *  => result: Ok(tuple("Hello", "World"), left: "")
 */

template<typename P1, typename P2, typename P3 = Success<Empty>, typename P4 = Success<Empty>, typename P5 = Success<Empty>,
		typename P6 = Success<Empty>, typename P7 = Success<Empty>, typename P8 = Success<Empty> >
class Sequence: public Parser<tuple<typename P1::data_type, typename P2::data_type, typename P3::data_type,
		typename P4::data_type, typename P5::data_type, typename P6::data_type,
		typename P7::data_type, typename P8::data_type> >
{
private:

	P1		_p1;
	P2		_p2;
	P3		_p3;
	P4		_p4;
	P5		_p5;
	P6		_p6;
	P7		_p7;
	P8		_p8;

public:
	typedef tuple<typename P1::data_type, typename P2::data_type, typename P3::data_type,
			typename P4::data_type, typename P5::data_type, typename P6::data_type,
			typename P7::data_type, typename P8::data_type>		data_type;
	typedef ParserResult<data_type>									result_type;

	Sequence(P1 p1, P2 p2, P3 p3 = Success<Empty>(), P4 p4 = Success<Empty>(), P5 p5 = Success<Empty>(),
			 P6 p6 = Success<Empty>(), P7 p7 = Success<Empty>(), P8 p8 = Success<Empty>()):
			_p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8) { }

	result_type		operator()(const slice& input)
	{
		typename P1::result_type	r1 = _p1(input);
		if (r1.is_err())
			return r1.unwind(input, "Sequence").template convert<data_type>();
		typename P2::result_type	r2 = _p2(r1.left());
		if (r2.is_err()) {
			result_type res = r2.unwind(input, "Sequence").template convert<data_type>();
			return r1.left().p != input.p ? res.failure() : res;
		}

		typename P3::result_type	r3 = _p3(r2.left());
		if (r3.is_err()) {
			result_type res = r3.unwind(input, "Sequence").template convert<data_type>();
			return r2.left().p != input.p ? res.failure() : res;
		}

		typename P4::result_type	r4 = _p4(r3.left());
		if (r4.is_err()) {
			result_type res = r4.unwind(input, "Sequence").template convert<data_type>();
			return r3.left().p != input.p ? res.failure() : res;
		}

		typename P5::result_type	r5 = _p5(r4.left());
		if (r5.is_err()) {
			result_type res = r5.unwind(input, "Sequence").template convert<data_type>();
			return r4.left().p != input.p ? res.failure() : res;
		}

		typename P6::result_type	r6 = _p6(r5.left());
		if (r6.is_err()) {
			result_type res = r6.unwind(input, "Sequence").template convert<data_type>();
			return r5.left().p != input.p ? res.failure() : res;
		}

		typename P7::result_type	r7 = _p7(r6.left());
		if (r7.is_err()) {
			result_type res = r7.unwind(input, "Sequence").template convert<data_type>();
			return r6.left().p != input.p ? res.failure() : res;
		}

		typename P8::result_type	r8 = _p8(r7.left());
		if (r8.is_err()) {
			result_type res = r8.unwind(input, "Sequence").template convert<data_type>();
			return r7.left().p != input.p ? res.failure() : res;
		}
		return result_type::ok(r8.left(), data_type(r1.unwrap(), r2.unwrap(), r3.unwrap(), r4.unwrap(), r5.unwrap(), r6.unwrap(), r7.unwrap(), r8.unwrap()));
	}
};

template<typename P1, typename P2>
Sequence<P1, P2, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty> >
sequence(P1 p1, P2 p2)
{
	Success<Empty>	p = Success<Empty>();
	return Sequence<P1, P2, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty> >(
			p1, p2, p, p, p, p, p, p);
}

template<typename P1, typename P2, typename P3>
Sequence<P1, P2, P3, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty> >
sequence(P1 p1, P2 p2, P3 p3)
{
	Success<Empty>	p = Success<Empty>();
	return Sequence<P1, P2, P3, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty> >(
			p1, p2, p3, p, p, p, p, p);
}

template<typename P1, typename P2, typename P3, typename P4>
Sequence<P1, P2, P3, P4, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty> >
sequence(P1 p1, P2 p2, P3 p3, P4 p4)
{
	Success<Empty>	p = Success<Empty>();
	return Sequence<P1, P2, P3, P4, Success<Empty>, Success<Empty>, Success<Empty>, Success<Empty> >(
			p1, p2, p3, p4, p, p, p, p);
}

template<typename P1, typename P2, typename P3, typename P4, typename P5>
Sequence<P1, P2, P3, P4, P5, Success<Empty>, Success<Empty>, Success<Empty> >
sequence(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
	Success<Empty> failure;
	return Sequence<P1, P2, P3, P4, P5, Success<Empty>, Success<Empty>,
			Success<Empty> >(p1, p2, p3, p4, p5, failure, failure, failure);
}

template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
Sequence<P1, P2, P3, P4, P5, P6, Success<Empty>, Success<Empty> >
sequence(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
{
	Success<Empty> failure;
	return Sequence<P1, P2, P3, P4, P5, P6, Success<Empty>, Success<Empty> >(p1, p2, p3, p4, p5, p6, failure, failure);
}

template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
Sequence<P1, P2, P3, P4, P5, P6, P7, Success<Empty> >
sequence(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
{
	return Sequence<P1, P2, P3, P4, P5, P6, P7, Success<Empty> >(p1, p2, p3, p4, p5, p6, p7, Success<Empty>());
}

template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
Sequence<P1, P2, P3, P4, P5, P6, P7, P8>
sequence(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
{
	return Sequence<P1, P2, P3, P4, P5, P6, P7, P8>(p1, p2, p3, p4, p5, p6, p7, p8);
}

#endif //WEBSERV_SEQUENCE_HPP
