#include "Http.hpp"

#include <unistd.h>

#include <iostream>

http::Http::Http(int socket, cfg::Configs *configs) : _socket(socket), _configs(configs)
{
	(void) _configs;
	(void) _socket;
}

http::Http::~Http()
{

}

void http::Http::readSocket()
{
	std::cout << "Http readSocker called" << std::endl;
	// int bytez;
	// char buffer[HTTP_BUFFER];

	// while (true) {
	// 	bytez = read(_socket, buffer, HTTP_BUFFER - 1);
	// 	if (bytez < 1) break;
	// 	_data.write(buffer, bytez);
	// 	break ;
	// }

	//debug
	// std::cout << _data.str() << std::endl;
}
