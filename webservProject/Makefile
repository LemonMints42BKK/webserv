NAME = webserve

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

PARSER_PATH = parser
HTTP_PATH = http
NETWORK_PATH = network

# HEADERS = ./parser/Configs.hpp ./network/Server.hpp ./http/Http.hpp

INCLUDE = -I$(PARSER_PATH) \
			-I$(HTTP_PATH) \
			-I$(NETWORK_PATH)

LIB = -Lparser -lparser \
		-Lnetwork -lnetwork \
		-Lhttp -lhttp

SRCS = main.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(OBJS): %.o: %.cpp Makefile
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(NAME): $(OBJS)
	make -C parser
	make -C network
	make -C http
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(LIB) -o $(NAME)

clean:
	make clean -C parser
	make clean -C network
	make clean -C http
	rm -f $(OBJS)

fclean: clean
	make fclean -C parser
	make fclean -C network
	make fclean -C http
	rm -f webserve

re: fclean all
