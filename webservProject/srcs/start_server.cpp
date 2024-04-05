// this file for angle Prem

#include "Server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include <stdexcept>
#include <iostream>

// this task should throw any error when unexpect
// when socket ready to read the function call __start_http(int socket);
void Server::start_server()
{
	int res;
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) throw(std::runtime_error("Error can not create server_socket"));

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5001);

	res = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
	if (res == -1) throw(std::runtime_error("Error can not bind server_socket"));

	res = listen(server_socket, 10);
	if (res == -1) throw(std::runtime_error("Error can not listen server_socket"));

	std::cout << "Server listen on port: " << 5001 << std::endl; 

	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(struct sockaddr_in);
	memset(&client_addr, 0, client_len);
	res = accept(server_socket,(struct sockaddr*)&client_addr , &client_len);
	if (res == -1) throw(std::runtime_error("Error can not accept server_socket"));

	std::cout << "Server accept on socket: " << res << std::endl;

	__start_http(res);
}
