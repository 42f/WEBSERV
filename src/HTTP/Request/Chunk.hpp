//
// Created by alena on 31/07/2021.
//

#ifndef WEBSERV_CHUNK_HPP
#define WEBSERV_CHUNK_HPP

#include "parser/export.hpp"
#include "parsing/Tokens.hpp"
#include "HTTP/Headers/HeaderParser.hpp"

//TODO cpp
struct chunk_data
{
public:
	slice	data;
	bool	end;

	explicit chunk_data(const slice &from): data(from), end(false) { }
	explicit chunk_data(bool end): data(slice()), end(end) { }

	static chunk_data	last(slice in)
	{
		(void)in;
		return chunk_data(true);
	}
};

/*
 * chunk-extension = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
 */
class ChunkExtension : public Parser<slice>
{
public:
	ChunkExtension();

	result_type operator()(const slice &input);
};

/*
 * chunk = chunk-size [ chunk-extension ] CRLF chunk-data CRLF
 */
class Chunk: public Parser<chunk_data>
{
public:
	Chunk();

	result_type		operator()(const slice&input);
};

/*
 * last-chunk = 1*("0") [ chunk-extension ] CRLF trailer CRLF
 */
class LastChunk: public Parser<chunk_data>
{
public:
	LastChunk();

	result_type		operator()(const slice&input);
};

/*
 * Chunked-Body = *chunk last-chunk
 */
class ChunkBody: public Parser<std::vector<chunk_data> >
{
public:
	ChunkBody();

	result_type		operator()(const slice &input);
};


#endif //WEBSERV_CHUNK_HPP
