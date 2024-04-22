#include "Server.hpp"

using namespace server;

#include <cstring>
// inconstruction
// default constructor it take default configuration to init server
Server::Server()
{
	__init_config();
}


Server::Server(cfg::Configs *configs)
{
	_configs = configs;
}
// destructor
Server::~Server(){}
