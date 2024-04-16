#include "Server.hpp"

#include <fcntl.h>
void Server::__init_config()
{
	// _config.listen["10.11.5.1"].push_back("5001"); machine school
	// _config.listen["172.17.0.1"].push_back("5001"); //my machine
	// _config.listen["0.0.0.0"].push_back("5002");
	_end_server = false;
	log_output = open("log_output.txt", O_CREAT | O_RDWR | O_APPEND, 0644);
	_config.root = "./home/html";
	_config.index.push_back("index.html");
}
