#include "Server.hpp"
#include <unistd.h>
#include <stdexcept>
#include <iostream>

// this task should throw any error when unexpect
// this version test only osi layer 4
void Server::__start_http(int socket)
{
	char buffer[1024];
	int bytez;

	bytez = read(socket, buffer, 1024);
	if (bytez < 0) throw (std::runtime_error("Error: read socket"));

	std::cout << "Message form client: " << std::endl;
	bytez = write(STDOUT_FILENO, buffer, bytez);
	if (bytez < 0) throw (std::runtime_error("Error: write socket"));

	bytez = write(socket, buffer, bytez);
	if (bytez < 0) throw (std::runtime_error("Error: write socket"));
}
