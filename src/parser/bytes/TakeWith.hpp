//
// Created by alena on 24/06/2021.
//

#ifndef WEBSERV_TAKEWITH_HPP
#define WEBSERV_TAKEWITH_HPP

/*
 * Matches everything that the parser returns true for
 * Eg: TakeWith(Digit()) ("12345Hello")
 *  => result: Ok("12345", left : "Hello")
 */
template<typename P>
class TakeWith: public Parser<slice>
{
private:
	P		_p;
	bool	_empty;

public:
	TakeWith(P parser, bool empty) : _p(parser), _empty(empty) { }

	result_type operator()(const slice &input)
	{
		slice	res = input.take(0);
		slice	left = input.from(0);
		typename P::result_type	r = P::result_type::err(input, error("", status::None));

		do
		{
			r = _p(left);
			if (r.is_err())
				break ;
			left = r.left();
			res = input.take(left.p - input.p);
		} while (left.size);
		if (res.size == 0 && _empty && input.size == 0)
			return result_type::ok(left, res);
		if (r.is_failure())
			return r.template convert<data_type>().unwind(input, "Take with: failure while parsing");
		if (res.size == 0 && !_empty)
			return result_type::err(input, error("TakeWith: no match"));
		return result_type::ok(left, res);
	}
};

template<typename P>
TakeWith<P>	take_with(P p, bool empty = false)
{
	return TakeWith<P>(p, empty);
}

#endif //WEBSERV_TAKEWITH_HPP
