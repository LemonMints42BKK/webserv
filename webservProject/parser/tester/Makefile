NAME = webserve

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

Include = ../

SRCS =	main.cpp

HEADERS = $(Include)Configs.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)
	make -C ..
	./$(NAME)

$(OBJS): %.o: %.cpp $(HEADERS) Makefile
	$(CC) $(CFLAGS) -I$(Include) -c $< -o $@

$(NAME): $(OBJS)
	make -C ..
	$(CC) $(CFLAGS) -I$(Include) $(SRCS) -L$(Include) -lparser -o $(NAME)

clean:
	make -C .. clean
	rm -f $(OBJS)

fclean: clean
	make -C .. fclean
	rm $(NAME)

re: fclean all

test:
	make -C ..
	make -C .. 
	cp ../libparser.a libparser.a
	make clean -C ..
	cmake -S . -B build
	cmake --build build
	cd build && ctest
