//
// Created by alena on 06/07/2021.
//

#ifndef WEBSERV_LISTEN_HPP
#define WEBSERV_LISTEN_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"


/*
 * DecOctet = 0-255
 */
class DecOctet : public Parser<slice>
{
public :
	DecOctet();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */

/*
 * AddressIP = dec-octet "." dec-octet ". dec-octet "." dec-octet
 */
class AddressIP : public Parser<std::string>
{
public :
	AddressIP();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */


/*
 * Port = 0-65535
 */
class Port : public Parser<int>
{
public:
	Port();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */


/*
 * Listen = listen RWS [ AdresseIP:port | port | Adresse IP ]
 */
class Listen : public Parser<tuple<std::string, int> >
{
private:
	static data_type with_port(int port);
	static data_type with_addr(std::string addr);

public :
	Listen();

	result_type operator()(const slice &input);
};

/* ************************************************************************** */


#endif //WEBSERV_LISTEN_HPP
