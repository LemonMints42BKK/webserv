NAME = network.a

INCLUDE = include

CC = c++
CFLAGS = -Wall -Werror -Wextra -I$(INCLUDE) -std=c++98

SRCS = 	srcs/Server.cpp \
		srcs/start_server.cpp \
		srcs/start_http.cpp \
		srcs/utils.cpp 

HEADERS = include/Server.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(OBJS): %.o: %.cpp $(HEADERS) Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS) ../parser/libparser.a 
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

client_huge:
	c++ ./client/client_1.cpp -o client_huge

client_normal:
	cc ./client/client.c -o client_normal

run:
	make -C ../parser
	make
	c++ ./srcs/main.cpp network.a ../parser/libparser.a 
	./a.out
