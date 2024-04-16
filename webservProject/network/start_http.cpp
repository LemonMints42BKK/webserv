#include "Server.hpp"

#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <fstream>
#include <signal.h>


// this task should throw any error when unexpect
// this version test only osi layer 4
void Server::__start_http(int socketfd)
{
	int rc, len;
	char buffer[65000];
	std::string request_line;
	std::string temp;
//    __setBlocking(i);

// _http[socketfd].read_socket;
// while will replace http::read_Socket
while (true)
{
		// setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, NULL, 0);
		rc = recv(socketfd, buffer, sizeof(buffer), MSG_WAITALL);
		std::cout << "HTTP: " << rc << std::endl;
		if (rc < 0)
			break;
		if (rc == 0)
		{
			printf("  Connection closed\n");
			_close_conn = true;
			break;
		}
		len = rc;
		_http[socketfd]->incrementTotalBytes(len);
		_http[socketfd]->sendResponse();
}


}

// void Server::__start_http(int socket)
// {
// 	// char buffer[65000];
// 	// int bytez = 0;
// 	std::vector<std::string> request;
// 	// int rc;
// 	// int sum = 0;
// 	std::string request_line;

// 	std::ofstream outfile("/home/spipitku/goinfre/outputFromserver.txt", std::ios::binary); // Create the output file
// 		// int flags = fcntl(socket, F_GETFL, 0);
// 		// if (flags < 0) {
// 		// 	perror("fcntl F_GETFL failed");
// 		// }
// 		// if (fcntl(socket, F_SETFL, flags & ~O_NONBLOCK) < 0) {
// 		// 	perror("fcntl F_SETFL failed");
// 		// }	
// 	if(socket == 6)
// 	{
// 		// int bytez = 0;

// 		std::string message = "Hello this message sent from PTP";
// 		std::ostringstream oss;
// 		oss << "HTTP/1.1 200 OK" 
// 			<< "Server: PTP"
// 			<< "Content-Type: text/plan;"
// 			<< "Content-Length: " << message.length()
// 			<< std::endl
// 			<< std::endl
// 			<< message; 

// 		send(socket, oss.str().c_str(), oss.str().length(), 0);
// 	}
// 	else
// 	{
// 		char buffer[65000];
// 		int rc;

// 		while (1)
// 		{
// 			rc = recv(socket, buffer, sizeof(buffer), 0);
// 			// printf("  rc = %d | socket = %d\n", rc, socket);
// 			if (rc < 0)
// 			{
// 				// if (errno != EWOULDBLOCK)
// 				// {
// 				perror("  recv() failed");
// 				_close_conn = true;
// 				break;
// 			}
// 			if (rc == 0)
// 			{
// 				// printf("  Connection closed\n");
// 				_close_conn = true;
// 				break;
// 			}
// 			// sum += rc;
			
// 			// buffer[rc + 1] = '\0';
// 			request_line.append(buffer, rc);
// 			// request.push_back(temp);
// 			// std::cout << "  Received: " << buffer << std::endl;
// 		}

// 		// std::cout << request_line << std::endl;
// 		// std::cout << "Message form client: " << sum << std::endl;
// 		// std::cout << "Message form client: " << request.size() << std::endl;
// 		// std::cout << "Message form client: " << request_line.length() << std::endl;
// 		// outfile << request_line;
// 		send(socket, "HTTP/1.1 200 OK\n", 16, 0);
// 		// write(socket, "HTTP/1.1 200 OK\n", 16);
// 	}
//  8077816
//  7947327

	// bytez = write(STDOUT_FILENO, buffer, bytez);
	// if (bytez < 0) throw (std::runtime_error("Error: write socket"));

	// std::string message = "Hello this message sent from PTP";
	// std::ostringstream oss;
	// oss << "HTTP/1.1 200 OK" 
	// 	<< "Server: PTP"
	// 	<< "Content-Type: text/plan;"
	// 	<< "Content-Length: " << message.length()
	// 	<< std::endl
	// 	<< std::endl
	// 	<< message; 

	// bytez = write(socket, oss.str().c_str(), oss.str().length());
	// if (bytez < 0) throw (std::runtime_error("Error: write socket"));

// }
