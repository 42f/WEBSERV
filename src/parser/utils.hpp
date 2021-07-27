//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_PARSER_UTILS_HPP
#define WEBSERV_PARSER_UTILS_HPP

#include <string>
#include <typeinfo>

struct Empty
{
	friend std::ostream	&operator<<(std::ostream& stream, const Empty& t)
	{
		(void)t;
		return stream;
	}
};

/*
 * Generic tuple struct, holds up to 8 different types
 */
template<typename T1, typename T2, typename T3 = Empty, typename T4 = Empty, typename T5 = Empty, typename T6 = Empty, typename T7 = Empty, typename T8 = Empty>
struct tuple
{
public:
	T1		first;
	T2		second;
	T3		third;
	T4		fourth;
	T5		fifth;
	T6		sixth;
	T7		seventh;
	T8		eighth;

	tuple() { }
	tuple(T1 t1, T2 t2, T3 t3 = Empty(), T4 t4 = Empty(), T5 t5 = Empty(), T6 t6 = Empty(), T7 t7 = Empty(), T8 t8 = Empty()): first(t1), second(t2), third(t3), fourth(t4), fifth(t5), sixth(t6), seventh(t7), eighth(t8) { }

	friend std::ostream	&operator<<(std::ostream& stream, const tuple& tuple)
	{
		stream << tuple.first << ", " << tuple.second;
		if (typeid(tuple.third) != typeid(Empty))
			stream << ", " << tuple.third;
		if (typeid(tuple.fourth) != typeid(Empty))
			stream << ", " << tuple.fourth;
		if (typeid(tuple.fifth) != typeid(Empty))
			stream << ", " << tuple.fifth;
		if (typeid(tuple.sixth) != typeid(Empty))
			stream << ", " << tuple.sixth;
		if (typeid(tuple.seventh) != typeid(Empty))
			stream << ", " << tuple.seventh;
		if (typeid(tuple.eighth) != typeid(Empty))
			stream << ", " << tuple.eighth;
		return stream;
	}
};


template<typename T1, typename T2>
tuple<T1, T2, Empty, Empty, Empty, Empty, Empty, Empty >
make_tuple(T1 t1, T2 t2)
{
	Empty	t;
	return tuple<T1, T2, Empty, Empty, Empty, Empty, Empty, Empty >(
			t1, t2, t, t, t, t, t, t);
}

template<typename T1, typename T2, typename T3>
tuple<T1, T2, T3, Empty, Empty, Empty, Empty, Empty >
make_tuple(T1 t1, T2 t2, T3 t3)
{
	Empty	t;
	return tuple<T1, T2, T3, Empty, Empty, Empty, Empty, Empty >(
			t1, t2, t3, t, t, t, t, t);
}

template<typename T1, typename T2, typename T3, typename T4>
tuple<T1, T2, T3, T4, Empty, Empty, Empty, Empty >
make_tuple(T1 t1, T2 t2, T3 t3, T4 t4)
{
	Empty	t;
	return tuple<T1, T2, T3, T4, Empty, Empty, Empty, Empty >(
			t1, t2, t3, t4, t, t, t, t);
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
tuple<T1, T2, T3, T4, T5, Empty, Empty, Empty >
make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
{
	Empty t;
	return tuple<T1, T2, T3, T4, T5, Empty, Empty, Empty >(t1, t2, t3, t4, t5, t, t, t);
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
tuple<T1, T2, T3, T4, T5, T6, Empty, Empty >
make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
{
	Empty t;
	return tuple<T1, T2, T3, T4, T5, T6, Empty, Empty >(t1, t2, t3, t4, t5, t6, t, t);
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
tuple<T1, T2, T3, T4, T5, T6, T7, Empty >
make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
{
	return tuple<T1, T2, T3, T4, T5, T6, T7, Empty >(t1, t2, t3, t4, t5, t6, t7, Empty());
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
tuple<T1, T2, T3, T4, T5, T6, T7, T8>
make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
{
	return tuple<T1, T2, T3, T4, T5, T6, T7, T8>(t1, t2, t3, t4, t5, t6, t7, t8);
}



#endif //WEBSERV_UTILS_HPP
