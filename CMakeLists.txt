cmake_minimum_required(VERSION 3.6)
project(webserv)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -g2 -std=c++98")

include_directories(src/parser src/parsing src/utils src)

set(FILES
        src/main.cpp
        src/Constants.hpp

        src/utils/logger.hpp
        src/utils/logger.cpp
        src/Server.cpp
        src/Server.hpp

        src/parser/combinators.hpp
        src/parser/Error.cpp
        src/parser/Error.hpp
        src/parser/export.hpp
        src/parser/Parser.hpp
        src/parser/Result.hpp
        src/parser/slice.hpp
        src/parser/utils.hpp

        src/parser/bytes/Alpha.cpp
        src/parser/bytes/Alpha.hpp
        src/parser/bytes/Char.cpp
        src/parser/bytes/Char.hpp
        src/parser/bytes/Digit.cpp
        src/parser/bytes/Digit.hpp
        src/parser/bytes/Eof.cpp
        src/parser/bytes/Eof.hpp
        src/parser/bytes/HexChar.cpp
        src/parser/bytes/HexChar.hpp
        src/parser/bytes/ITag.cpp
        src/parser/bytes/Itag.hpp
        src/parser/bytes/Match.cpp
        src/parser/bytes/Match.hpp
        src/parser/bytes/OneOf.cpp
        src/parser/bytes/OneOf.hpp
        src/parser/bytes/Strip.hpp
        src/parser/bytes/Tag.cpp
        src/parser/bytes/Tag.hpp
        src/parser/bytes/TakeUntil.cpp
        src/parser/bytes/TakeUntil.hpp
        src/parser/bytes/TakeWhile.cpp
        src/parser/bytes/TakeWhile.hpp
        src/parser/bytes/TakeWith.hpp
        src/parser/bytes/TakeExact.cpp
        src/parser/bytes/TakeExact.hpp

        src/parser/combinators/Alt.hpp
        src/parser/combinators/AsSlice.hpp
        src/parser/combinators/Map.hpp
        src/parser/combinators/Opt.hpp
        src/parser/combinators/Wrap.hpp

        src/parser/sequence/Delimited.hpp
        src/parser/sequence/Many.hpp
        src/parser/sequence/Preceded.hpp
        src/parser/sequence/Separated.hpp
        src/parser/sequence/Sequence.hpp
        src/parser/sequence/Terminated.hpp



        src/parsing/Methods.cpp
        src/parsing/Methods.hpp
        src/parsing/Tokens.cpp
        src/parsing/Tokens.hpp



        src/HTTP/Request.cpp
        src/HTTP/Request.hpp
        src/HTTP/Status.cpp
        src/HTTP/Status.hpp
        src/HTTP/RequestParser.cpp
        src/HTTP/RequestParser.hpp
        src/HTTP/RequestHandler.cpp
        src/HTTP/RequestHandler.hpp
        src/HTTP/Request/RequestLine.cpp
        src/HTTP/Request/RequestLine.hpp
        src/HTTP/Request/Chunk.cpp
        src/HTTP/Request/Chunk.hpp
        src/HTTP/Request/RequestTarget/OriginForm.cpp
        src/HTTP/Request/RequestTarget/OriginForm.hpp
        src/HTTP/Request/RequestTarget/Segment.cpp
        src/HTTP/Request/RequestTarget/Segment.hpp
        src/HTTP/Request/RequestTarget/Scheme.cpp
        src/HTTP/Request/RequestTarget/Scheme.hpp
        src/HTTP/Request/RequestTarget/Target.hpp

        src/HTTP/Headers/Header.cpp
        src/HTTP/Headers/Header.hpp
        src/HTTP/Headers/HeaderParser.hpp
        src/HTTP/Headers/Headers.cpp
        src/HTTP/Headers/Headers.hpp



        src/Config/ConfigParser.cpp
        src/Config/ConfigParser.hpp
        src/Config/Server.cpp
        src/Config/Server.hpp
        src/Config/ServerBlock.cpp
        src/Config/ServerBlock.hpp

        src/Config/Directives/Cgi.cpp
        src/Config/Directives/Cgi.hpp
        src/Config/Directives/ClientMaxBodySize.cpp
        src/Config/Directives/ClientMaxBodySize.hpp
        src/Config/Directives/ErrorPage.cpp
        src/Config/Directives/ErrorPage.hpp
        src/Config/Directives/Index.cpp
        src/Config/Directives/Index.hpp
        src/Config/Directives/Listen.cpp
        src/Config/Directives/Listen.hpp
        src/Config/Directives/Location.cpp
        src/Config/Directives/Location.hpp
        src/Config/Directives/Methods.cpp
        src/Config/Directives/Methods.hpp
        src/Config/Directives/Redirect.cpp
        src/Config/Directives/Redirect.hpp
        src/Config/Directives/Root.cpp
        src/Config/Directives/Root.hpp
        src/Config/Directives/ServerName.cpp
        src/Config/Directives/ServerName.hpp
        src/Config/Directives/Upload.cpp
        src/Config/Directives/Upload.hpp

        src/Config/Structure/Bloc.hpp
        src/Config/Structure/Line.hpp
        src/Config/Structure/Line.cpp
        )

message("compiling with flags ${CMAKE_CXX_FLAGS}")

add_executable(webserv ${FILES})
set_property(TARGET webserv PROPERTY CXX_STANDARD 98)
