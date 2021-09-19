
NAME = webserv

SRCS :=	$(shell find src | grep ".cpp")

OBJ_DIR = objs/
SRC_DIR = src/

STANDARD =

OBJS = $(addprefix $(OBJ_DIR),$(patsubst src/%.cpp, %.o, $(SRCS)))

RM = rm -f

CC = clang++

CFLAGS= -Wall -Wextra -Werror -Isrc -Isrc/parser -Isrc/parsing -Isrc/HTTP -Isrc/utils -Isrc/HTTP/Request -Isrc/HTTP/Response -Isrc/Network

ifdef fast
else
	CFLAGS:= ${CFLAGS} -std=c++98
endif

all: ${NAME}

fast:
	make -j12 -f makefile.debug $(OBJS)
	make -f makefile.debug ${NAME}

print: $(wildcard src/**/*.cpp)
	ls -la $?

${NAME}: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LINK)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)

	@mkdir -p $(OBJ_DIR)/CGI


	@mkdir -p $(OBJ_DIR)parser
	@mkdir -p $(OBJ_DIR)parser/bytes
	@mkdir -p $(OBJ_DIR)parser/combinators
	@mkdir -p $(OBJ_DIR)parser/sequence

	@mkdir -p $(OBJ_DIR)Network

	@mkdir -p $(OBJ_DIR)Config
	@mkdir -p $(OBJ_DIR)Config/Directives
	@mkdir -p $(OBJ_DIR)Config/Structure

	@mkdir -p $(OBJ_DIR)HTTP
	@mkdir -p $(OBJ_DIR)HTTP/Headers
	@mkdir -p $(OBJ_DIR)HTTP/Request
	@mkdir -p $(OBJ_DIR)HTTP/Response
	@mkdir -p $(OBJ_DIR)HTTP/Request/RequestUtils
	@mkdir -p $(OBJ_DIR)HTTP/Response/ResponseUtils

	@mkdir -p $(OBJ_DIR)utils
	@mkdir -p $(OBJ_DIR)parsing

	${CC} ${CFLAGS} -c -o $@ $<

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all
	./assets/clean_setup_files.sh

.PHONY : clean fclean re
