#include "Server.hpp"

// inconstruction
// default constructor it take default configuration to init server
Server::Server()
{
	// this task should throw any error when unexpect

	// in production parser config
	// __parser_config("default");

	// in develop use init
	__init_config();
}

Server::Server(cfg::Configs *configs)
{
	_configs = configs;
}

Server::Server(const std::string name_file)
{
	_configs = new cfg::Configs(name_file);
}
// inconstruction
// copy constructor
Server::Server(Server const &ins)
{
	(void) ins;
}

// destructor
Server::~Server()
{

}

// inconstruction
// copy assignment constructor
Server & Server::operator=(Server const &rhs)
{
	(void) rhs;
	return (*this);
}
