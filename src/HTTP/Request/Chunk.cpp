//
// Created by alena on 31/07/2021.
//

#include "Chunk.hpp"

/*
 * chunk-extension = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
 *
 * chunk-ext-name = token
 * chunk-ext-val  = token | quoted-string
 */

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
ChunkExtension::ChunkExtension() { }
/*
** --------------------------------- METHODS ----------------------------------
*/
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
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Chunk::Chunk() { }
/*
** --------------------------------- METHODS ----------------------------------
*/
Chunk::result_type	Chunk::operator()(const slice&input)
{
	static const ChunkExtension EXT = ChunkExtension();
	static const OneOf HEX_DIGITS = OneOf("0123456789abcdefABCDEF");

	ParserResult<slice>	size = take_with(HEX_DIGITS)(input);

	if (size.is_err())
		return size.convert<chunk_data>().unwind(input, "chunk: invalid");
	size_t len = size.unwrap().to_int(16);
	if (len == 0)
		return result_type::err(input, error("chunk: end"));
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
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
LastChunk::LastChunk() { }
/*
** --------------------------------- METHODS ----------------------------------
*/
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
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
ChunkBody::ChunkBody() { }
/*
** --------------------------------- METHODS ----------------------------------
*/
ChunkBody::result_type	ChunkBody::operator()(const slice &input)
{
	ParserResult<std::vector<chunk_data> >	lst = many(Chunk())(input);

	if (lst.is_err())
		return lst;
	ParserResult<chunk_data>		end = LastChunk()(lst.left());
	if (end.is_ok())
	{
		std::vector<chunk_data>	&v = lst.unwrap();
		v.push_back(end.unwrap());
		return result_type::ok(end.left(), v);
	}
	return lst;
}

/* ************************************************************************** */
