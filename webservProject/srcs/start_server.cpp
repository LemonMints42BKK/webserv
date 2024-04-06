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
// this task should throw any error when unexpect
// when socket ready to read the function call __start_http(int socket);

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
            int flags = fcntl(sockfd, F_GETFL, 0); // Get current flags
            if (flags == -1) {
                perror("fcntl(F_GETFL)");
                // Handle error
            }
            flags |= O_NONBLOCK; // Set non-blocking flag
            int result = fcntl(sockfd, F_SETFL, flags);
            if (result == -1) {
                perror("fcntl(F_SETFL)");
                // Handle error
            }

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
            printf("Server listening on port %d\n", std::atoi(port.c_str()));
            return sockfd;
}

void Server::__getInputAndCreateListSocket(void)
{
	for (Listen it = _config.listen.begin(); it != _config.listen.end(); ++it) {
		const std::string& key = it->first;
		const std::vector<std::string>& value = it->second;
		for (unsigned int i = 0; i < value.size(); ++i) {
			socketlist.push_back(__createNewSocket(key, value.at(i)));
		}
	}
}

void Server::__setUpMoniterSocket(void)
{
	std::cout << "Set Up Moniter Socket" << std::endl;
	FD_ZERO(&_master_set);
	for (size_t i = 0; i < socketlist.size(); ++i) {
		FD_SET(socketlist[i], &_master_set);
	}
	_max_sd = socketlist.size() + 3;
}	

void Server::__runMoniter(void)
{
	do
	{
		int rc;
		std::memcpy(&_working_set, &_master_set, sizeof(_master_set));
		printf("Waiting on select()...\n");
		rc = select(_max_sd + 1, &_working_set, NULL, NULL,  NULL);
		if (rc < 0)
		{
			perror("select() failed");
			break;
		}
		if (rc == 0)
		{
			printf("select() timed out. End program.\n");
			break;
		}
		__loopCheckFd_workingSet();
	} while (_end_server == false);  
}

void Server::__loopCheckFd_workingSet(void)
{
	bool close_conn = false;
	for (size_t i = 0; i <= static_cast<size_t>(_max_sd); ++i)
	{
		if (FD_ISSET(i, &_working_set))
		{
			if (!(i == static_cast<size_t>(socketlist.at(0))))
			{
				// readDataFromClient(i,close_conn); // Read data from client
				try
				{
					__start_http(i);
				}
				catch(const std::exception& e)
				{		
					throw ;
				}
				close_conn = true;
			}
			__requestFromClient(); // Accept new client
			if (close_conn)
			{
				printf("Close fd : %lu\n",i);
				close(i);
				FD_CLR(i, &_master_set);
				if (i == static_cast<size_t>(_max_sd))
				{
					while (FD_ISSET(_max_sd, &_master_set) == false)
								_max_sd -= 1;
				}
			}
		}
	}/* End of loop through selectable descriptors */
}

void Server::__requestFromClient()
{
	int new_sd;
	do
	{
		new_sd = accept(socketlist.at(0), NULL, NULL);
		if (new_sd < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				perror("  accept() failed");
				_end_server = true;
			}
			break;
		}
		printf("  New incoming connection - %d\n", new_sd);
		FD_SET(new_sd, &_master_set);
		if (new_sd > _max_sd)
			_max_sd = new_sd;
	} while (new_sd != -1);
}

