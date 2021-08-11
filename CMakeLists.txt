cmake_minimum_required(VERSION 3.6)
project(webserv)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -Wall -Wextra -Werror -g2 -std=c++98")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g2")

include_directories(
        src
        src/HTTP
        src/HTTP/config
        src/HTTP/Headers
        src/HTTP/Request
        src/HTTP/Request/RequestTarget
        src/HTTP/Response
        src/HTTP/Response/ResponseUtils
        src/parser
        src/parsing
        src/Network
        src/utils)

set(FILES
        src/main.cpp
        src/Constants.hpp

        src/utils/logger.hpp
        src/utils/logger.cpp

        src/HTTP/Response/Response.cpp
        src/HTTP/Response/Response.hpp
        src/HTTP/Response/ResponseHandler.cpp
        src/HTTP/Response/ResponseHandler.hpp
        src/HTTP/Response/ResponseUtils/ResponseHeader.cpp
        src/HTTP/Response/ResponseUtils/ResponseHeader.hpp
        src/HTTP/Response/ResponseUtils/A_Method.hpp
        src/HTTP/Response/ResponseUtils/File.cpp
        src/HTTP/Response/ResponseUtils/File.hpp
        src/HTTP/Response/ResponseUtils/TypesMime.cpp
        src/HTTP/Response/ResponseUtils/TypesMime.hpp

        src/Network/ServerPool.hpp
        src/Network/ServerPool.cpp
        src/Network/ServerSocket.hpp
        src/Network/ServerSocket.cpp
        src/Network/Socket.hpp
        src/Network/Socket.cpp
        src/Network/Core.hpp
        src/Network/Core.cpp
        src/Network/EventManager.hpp
        src/Network/EventManager.cpp

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


        src/HTTP/Request/Request.cpp
        src/HTTP/Request/Request.hpp
        src/HTTP/Status.cpp
        src/HTTP/Status.hpp
        src/HTTP/Request/RequestParser.cpp
        src/HTTP/Request/RequestParser.hpp
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
