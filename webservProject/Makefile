
all:
	make -C parser
	make -C network
	c++ main_test.cpp ./parser/parser_webserve.a ./network/network.a -o webserve

clean:
	make clean -C parser
	make clean -C network

fclean: clean
	make fclean -C parser
	make fclean -C network
	rm -f webserve