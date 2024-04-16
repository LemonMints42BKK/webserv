NAME = libhttp.a

PARSER_PATH = ../parser

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

INCLUDES = -I$(PARSER_PATH)

SRCS =	Http.cpp \
		Httptest.cpp \
		HttpV1.cpp

HEADERS = Http.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(OBJS): %.o: %.cpp $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm $(NAME)

re: fclean all

