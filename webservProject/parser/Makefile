NAME = libparser.a

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS =	AConfig.cpp \
		AConfigs.cpp \
		AGroup.cpp \
		AConfigString.cpp \
		AConfigInt.cpp \
		AConfigVectorString.cpp \
		AConfigPairStringString.cpp \
		Configs.cpp \
		Worker_processes.cpp \
		Client_max_body.cpp \
		Http.cpp \
		Server.cpp \
		Server_name.cpp \
		Listen.cpp \
		Index.cpp \
		Root.cpp \
		Redirect.cpp \
		Error_page.cpp \
		Location.cpp \
		Allow.cpp \
		Types.cpp \
		Type.cpp \
		Cgi_file.cpp \
		Cgi_exe.cpp

HEADERS = Configs.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(OBJS): %.o: %.cpp $(HEADERS) Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
