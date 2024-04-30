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

int http::Http::getSocket()const
{
	return _socket;
}
