NAME = webserve

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS = srcs/main.cpp srcs/Server.cpp srcs/start_server.cpp srcs/start_http.cpp srcs/utils.cpp

HEADERS = srcs/Server.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)
	./$(NAME)

$(OBJS): %.o: %.cpp $(HEADERS) Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm $(NAME)

re: fclean all
