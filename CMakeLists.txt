cmake_minimum_required(VERSION 3.6)
project(webserv)

# TODO remove fsanitize for evaluation !
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -O3 -g3 -std=c++98")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -g3 -std=c++98")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -Wall -Wextra -Werror -g2 -std=c++98")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g2 -std=c++98")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address -std=c++98")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -std=c++98")

include_directories(
        src
        src/HTTP
        src/HTTP/config
        src/HTTP/Headers
        src/HTTP/Request
        src/HTTP/Request/RequestUtils
        src/HTTP/Response
        src/HTTP/Response/ResponseUtils
        src/parser
        src/parsing
        src/Network
        src/CGI
        src/utils)

set(FILES
        src/main.cpp
        src/Constants.hpp

        src/utils/Timer.hpp
        src/utils/Logger.hpp
        src/utils/Logger.cpp

        src/HTTP/Response/Response.cpp
        src/HTTP/Response/Response.hpp
        src/HTTP/Response/Autoindex.hpp
        src/HTTP/Response/ResponseHandler.cpp
        src/HTTP/Response/ResponseHandler.hpp
        src/HTTP/Response/ResponseUtils/ResponseHeader.cpp
        src/HTTP/Response/ResponseUtils/ResponseHeader.hpp
        src/HTTP/Response/ResponseUtils/File.cpp
        src/HTTP/Response/ResponseUtils/File.hpp

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

        src/CGI/CGI.hpp
        src/CGI/CGI.cpp

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
        src/HTTP/Request/RequestHandler.cpp
        src/HTTP/Request/RequestHandler.hpp
        src/HTTP/Request/RequestUtils/RequestLine.cpp
        src/HTTP/Request/RequestUtils/RequestLine.hpp
        src/HTTP/Request/RequestUtils/Chunk.cpp
        src/HTTP/Request/RequestUtils/Chunk.hpp
        src/HTTP/Request/RequestUtils/Path.cpp
        src/HTTP/Request/RequestUtils/Path.hpp
        src/HTTP/Request/RequestUtils/Charset.cpp
        src/HTTP/Request/RequestUtils/Charset.hpp
        src/HTTP/Request/RequestUtils/Scheme.cpp
        src/HTTP/Request/RequestUtils/Scheme.hpp
        src/HTTP/Request/RequestUtils/Target.hpp

        src/HTTP/Headers/Header.cpp
        src/HTTP/Headers/Header.hpp
        src/HTTP/Headers/HeaderParser.hpp
        src/HTTP/Headers/Headers.cpp
        src/HTTP/Headers/Headers.hpp



        src/Config/ConfigParser.cpp
        src/Config/ConfigParser.hpp
        src/Config/Server.cpp
        src/Config/Server.hpp
        src/Config/Directives/ServerBlock.cpp
        src/Config/Directives/ServerBlock.hpp

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
