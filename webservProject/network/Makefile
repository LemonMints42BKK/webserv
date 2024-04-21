NAME = libnetwork.a

PARSER_PATH = ../parser
HTTP_PATH = ../http

INCLUDES = -I$(PARSER_PATH) -I$(HTTP_PATH)

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS = 	Server.cpp \
		start_server.cpp \
		utils.cpp \
		createNewSocket.cpp
#		start_http.cpp \

HEADERS = Server.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(OBJS): %.o: %.cpp $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

# client_huge:
# 	c++ ./client/client_1.cpp -o client_huge

client_normal:
	cc ./client/client.c -o client_normal

# run:
# 	make -C ../parser
# 	make
# 	c++ ./srcs/main.cpp network.a ../parser/libparser.a 
# 	./a.out
