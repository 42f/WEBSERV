//
// Created by alena on 14/07/2021.
//

#include "Listen.hpp"
#include <cstdlib>

/*
 * DecOctet = 0-255
 */
DecOctet::DecOctet() { }

DecOctet::result_type DecOctet::operator()(const slice &input)
{
	ParserResult<slice> res = TakeWhile(std::isdigit)(input);
	if (res.is_err())
		return res;
	int n = atoi(res.unwrap().p);
	if (n > 255)
		return result_type::fail(res.left(), error("Octet: Too big"));
	return res;
}

/* ************************************************************************** */

/*
 * AddressIP = dec-octet "." dec-octet ". dec-octet "." dec-octet
 */
AddressIP::AddressIP() { }

AddressIP::result_type AddressIP::operator()(const slice &input)
{
	return map(as_slice(sequence(
				DecOctet(), Char('.'),
				DecOctet(), Char('.'),
				DecOctet(), Char('.'),
				DecOctet())), slice::to_string_static)(input);
}

/* ************************************************************************** */

/*
 * Port = 0-65535
 */
Port::Port() { }

Port::result_type Port::operator()(const slice &input)
{
	ParserResult<slice> res = take_with(digit)(input);
	if (res.is_err())
		return res.convert<int>();
	int port = res.unwrap().to_int();
	if (port > 65535)
		return result_type::fail(res.left(), error("port must be between 0 and 65535"));
	return result_type::ok(res.left(), port);
}

/* ************************************************************************** */

/*
 * Listen = listen RWS [ AdresseIP:port | port | Adresse IP ]
 */
Listen::Listen() { }

Listen::data_type	Listen::with_port(int port) { return make_tuple(std::string("0.0.0.0"), port); }
Listen::data_type	Listen::with_addr(std::string addr) { return make_tuple(addr, 8000); }

Listen::result_type Listen::operator()(const slice &input)
{
	return preceded(sequence(Tag("listen"), rws), alt(
			separated(AddressIP(), Tag(":"), Port()),
			map(AddressIP(), with_addr),
			map(Port(), with_port)
	))(input);
}

/* ************************************************************************** */

