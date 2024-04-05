#include "Server.hpp"
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

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

	std::string message = "Hello this message sent from PTP";
	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK" 
		<< "Server: PTP"
		<< "Content-Type: text/plan;"
		<< "Content-Length: " << message.length()
		<< std::endl
		<< std::endl
		<< message; 

	bytez = write(socket, oss.str().c_str(), oss.str().length());
	if (bytez < 0) throw (std::runtime_error("Error: write socket"));
}
