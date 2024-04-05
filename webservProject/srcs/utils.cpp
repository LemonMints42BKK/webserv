#include "Server.hpp"

void Server::__init_config()
{
	_config.listen[0].push_back(5001);
	_config.listen[0].push_back(5002);
	// _config.listen[100000].push_back(PORT);
	// _config.listen[100000].push_back(PORT + 1);
	_config.root = "./home/html";
	_config.index.push_back("index.html");
}
