#include "Server.hpp"

#include <fcntl.h>
#include <cstring>
using namespace server;

void Server::__init_config()
{
	std::memset(&_time, 0, sizeof(*_time) * FD_SETSIZE);
	std::memset(&_http, 0, sizeof(*_http) * FD_SETSIZE);
	_end_server = false;
}
