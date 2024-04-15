// this file for angle Prem

#include "Server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdio.h>
#include <cstdlib>
#include <errno.h>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sys/wait.h>

// this task should throw any error when unexpect
// when socket ready to read the function call __start_http(int socket);

void print_fd_set(fd_set *set) {
    printf("FD_SET contents (binary representation):\n");
    unsigned char *set_ptr = (unsigned char *)set;
    for (int i = 0; i < 2; ++i) {
        for (int j = 7; j >= 0; --j) {
            printf("%d", (set_ptr[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

void sigio_handler(int signum) {
	static_cast<void>(signum);
    printf("SIGIO received\n");
    // Handle asynchronous I/O operation here
}

void Server::start_server()
{
	__getInputAndCreateListSocket();
	__setUpMoniterSocket();
	__runMoniter();
}


int Server::__createNewSocket(std::string ip ,std::string port)
{
            int rc,on=1;
            
            //Hotel Opens (Server Starts): You create a socket using socket().
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                perror("socket() failed");
                exit(-1);
            }

            //Set up hotel for serve client this it set up for client use a seem door
            rc = setsockopt(sockfd, SOL_SOCKET,  SO_REUSEADDR,
                    (char *)&on, sizeof(on));   
            if (rc < 0)
            {
                perror("setsockopt() failed");
                close(sockfd);
                exit(-1);
            }
            // Set socket for Nonblocking io
			__setNonBlocking(sockfd);

            // Set Up Rooms (Bind Socket): You inform the hotel staff (operating system) 
            // about the hotel's address (IP address) and room 
            // availability (port number) using bind().
            struct addrinfo hints, *res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;  // Request IPv4 addresses
            hints.ai_socktype = SOCK_STREAM;  // Request TCP sockets

            int status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res);
            if (status != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
                close(sockfd);
                return 1;
            }

            // Choose an address from the linked list (e.g., res->ai_addr)
            struct sockaddr_in server_addr;
            memcpy(&server_addr, res->ai_addr, res->ai_addrlen);  // Copy address info
            freeaddrinfo(res);  // Free memory allocated by getaddrinfo

            server_addr.sin_port = htons(std::atoi(port.c_str()));  // Convert port to network byte order
            // Bind the socket to the address and port
			std::cout << "Binding to " << ip << ":" << port << std::endl;
            if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
                perror("bind failed");
                exit(1);
            }
            // Prepare Lobby (Listen): You use listen() to specify the maximum number of 
            // guests (clients) who can wait in the lobby before new reservations are rejected.
             // Listen for incoming connections
            if (listen(sockfd, 5) == -1) {  // Backlog queue of size 5
                perror("listen failed");
                exit(1);
            }
            std::cout << "Server listening on " << ip << ":" << port << std::endl;
            return sockfd;
}

void Server::__getInputAndCreateListSocket(void)
{
	cfg::Listens listens;
	_configs->getListen_getIp(listens, _configs->begin(), _configs->end());
	for (Listen it = listens.begin(); it != listens.end(); ++it) {
		const std::string& key = it->first;
		const std::vector<std::string>& value = it->second;
		for (unsigned int i = 0; i < value.size(); ++i) {
			std::cout << key << " " << value.at(i) << std::endl;
			socketlist.push_back(__createNewSocket(key, value.at(i)));
		}
	}
}

void Server::__setUpMoniterSocket(void)
{
	// std::cout << "Set Up Moniter Socket" << std::endl;
	FD_ZERO(&_master_set);
	for (size_t i = 0; i < socketlist.size(); ++i) {
		FD_SET(socketlist[i], &_master_set);
	}
	_max_sd = socketlist.back();
}	

void Server::__runMoniter(void)
{
	// struct timeval timeout = {0, 500000}; // 0.5 seconds
	do
	{
		int rc;

		std::memcpy(&_working_set, &_master_set, sizeof(_master_set));
		printf("  Waiting on select()...\n");
		rc = select(_max_sd + 1, &_working_set, NULL, NULL,  NULL);
		if (rc < 0)
		{
			perror("select() failed");
			break;
		}
		__loopCheckFd_workingSet();
	} while (_end_server == false);  
}


void Server::__loopCheckFd_workingSet(void)
{
	// int sum = 0;
	_close_conn = false;
	for (size_t i = 0; i <= static_cast<size_t>(_max_sd); ++i)
	{
		bool is_socket_listen = std::find(socketlist.begin(), socketlist.end(), i) != socketlist.end();
		if (FD_ISSET(i, &_working_set))
		{
			if (is_socket_listen)
				__requestFromClient(static_cast<int>(i)); // Accept new client
			else
			{
				__start_http(i);
				__handle_close_conn(i);
			}
		}
	}/* End of loop through selectable descriptors */
}

void Server::__handle_close_conn(size_t socketfd)
{
	if (_close_conn)
	{
		printf("Close fd : %lu\n", socketfd);
		close(socketfd);
		FD_CLR(socketfd, &_master_set);
		delete _http[socketfd];
		if (socketfd == static_cast<size_t>(_max_sd))
		{
			while (FD_ISSET(_max_sd, &_master_set) == false)
						_max_sd -= 1;
		}
	}
}

void Server::__setCloseSocketFdListen(void)
{
	for (size_t i = 0; i < socketlist.size(); ++i)
	{
		close(socketlist[i]);
		FD_CLR(socketlist[i], &_master_set);
	}	
}


bool Server::__getCheckMaster_AllZero(void)
{
	for (size_t i = 0; i <= static_cast<size_t>(_max_sd); ++i)
	{
		if (FD_ISSET(i, &_master_set))
			return false;
	}
	return true;
}

void Server::__requestFromClient(int socket)
{
	int new_sd;
	do
	{
		new_sd = accept(socket, NULL, NULL);
		if (new_sd < 0)
			break;
		__setNonBlocking(new_sd);
		printf("  New incoming connection %d\n", new_sd);
		_http[new_sd] = new Http(new_sd);
		FD_SET(new_sd, &_master_set);
		if (new_sd > _max_sd)
			_max_sd = new_sd;
		printf("  New max_sd %d\n", _max_sd);
	} while (new_sd != -1);
}

void Server::__setNonBlocking(int socket)
{
	int result = fcntl(socket, F_SETFL, O_NONBLOCK);
	if (result == -1) {
		perror("fcntl(F_SETFL)");
		// Handle error
	}
}