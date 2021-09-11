//
// Created by alena on 31/07/2021.
//

#include "Chunk.hpp"

Chunk::result_type	chunk_errors(Chunk::result_type err)
{
	if (err.is_err() && err.unwrap_err().content() != status::Incomplete)
		return err.failure();
	return err;
}

/*
 * chunk-extension = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
 *
 * chunk-ext-name = token
 * chunk-ext-val  = token | quoted-string
 */
ChunkExtension::ChunkExtension() { }

ChunkExtension::result_type	ChunkExtension::operator()(const slice &input)
{
	return as_slice(many(preceded(Char(';'),
						sequence(token,opt(preceded(Char('='),
							alt(token, quoted_text))))), true))(input);
}

/* ************************************************************************** */

/*
 * chunk = chunk-size [ chunk-extension ] CRLF chunk-data CRLF
 *
 * chunk-size = 1*HEX
 * chunk-data = chunk-size(OCTET)
 */
Chunk::Chunk() { }

Chunk::result_type	Chunk::operator()(const slice&input)
{
	static const ChunkExtension EXT = ChunkExtension();
	static const OneOf HEX_DIGITS = OneOf("0123456789abcdefABCDEF");

	ParserResult<slice>	size = take_with(HEX_DIGITS)(input);

	if (size.is_err())
		return size.convert<chunk_data>().unwind(input, "chunk: invalid");
	size_t len = size.unwrap().to_int(16);
	if (len == 0)
		return result_type::err(input, error("chunk : end", status::Incomplete));
	ParserResult<slice>	data = preceded(opt(EXT), delimited(Newline(),
				TakeExact(len), Newline()))(size.left());
	if (data.is_err())
		return data.convert<chunk_data>().unwind(input, "chunk: invalid");
	return result_type::ok(data.left(), chunk_data(data.unwrap()));
}

/* ************************************************************************** */

/*
 * last-chunk = 1*("0") [ chunk-extension ] CRLF trailer CRLF
 */
LastChunk::LastChunk() { }

LastChunk::result_type	LastChunk::operator()(const slice&input)
{
	return terminated(map(take_with(Char('0')),chunk_data::last),
				sequence(opt(ChunkExtension()), Newline(),
					many(HeaderParser<>(), true), Newline()))(input);
}

/* ************************************************************************** */

/*
 * Chunked-Body = *chunk last-chunk
 */
ChunkBody::ChunkBody() { }

ChunkBody::result_type	ChunkBody::operator()(const slice &input)
{
	ParserResult<std::vector<chunk_data> >	lst = many(
		map_err(Chunk(), chunk_errors), true)(input);

	if (lst.is_failure())
		return lst.convert<std::vector<chunk_data> >().unwind(input, "ChunkBody: parsing failed");
	ParserResult<chunk_data>		end = LastChunk()(lst.left());
	if (end.is_ok())
	{
		std::vector<chunk_data>	&v = lst.unwrap();
		v.push_back(end.unwrap());
		return result_type::ok(end.left(), v);
	}
	else if (end.is_err() && end.left() != lst.left())
		return end.convert<std::vector<chunk_data> >().unwind(input, "ChunkBody: end chunk parsing failed");
	return lst;
}

/* ************************************************************************** */
