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

LIB = -L$(PARSER_PATH) -l$(PARSER_PATH) \
		-L$(NETWORK_PATH) -l$(NETWORK_PATH) \
		-L$(HTTP_PATH) -l$(HTTP_PATH)

LIB_SRCS = $(HTTP_PATH)/*.cpp $(PARSER_PATH)/*.cpp $(NETWORK_PATH)/*.cpp 

SRCS = main.cpp

OBJS = $(SRCS:.cpp=.o)

# NETWORK_SRCS = $(wildcard $(NETWORK_PATH)/*.cpp)
# PARSER_SRCS = $(wildcard $(PARSER_PATH)/*.cpp)
# HTTP_SRCS = $(wildcard $(HTTP_PATH)/*.cpp)

all: $(NAME)

m:
	./$(NAME) configs/default.conf


$(OBJS): %.o: %.cpp Makefile $(LIB_SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(NAME): $(OBJS)
	make -C $(PARSER_PATH)
	make -C $(NETWORK_PATH)
	make -C $(HTTP_PATH)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(LIB) -o $(NAME)

clean:
	make clean -C $(PARSER_PATH)
	make clean -C $(NETWORK_PATH)
	make clean -C $(HTTP_PATH)
	rm -f $(OBJS)

fclean: clean
	make fclean -C $(PARSER_PATH)
	make fclean -C $(NETWORK_PATH)
	make fclean -C $(HTTP_PATH)
	rm -f webserve

re: fclean all
