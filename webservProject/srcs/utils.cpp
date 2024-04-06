#include "Server.hpp"

void Server::__init_config()
{
	// _config.listen["10.11.5.1"].push_back("5001");
	_config.listen["0.0.0.0"].push_back("5002");
	_end_server = false;
	_config.root = "./home/html";
	_config.index.push_back("index.html");
}
