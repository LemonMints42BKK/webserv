
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

using namespace server;

void Server::__getNewSocketfd(int &sockfd)
{
    //Hotel Opens (Server Starts): You create a socket using socket().
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        close(sockfd);
        __closeAllSocketlist();
        throw std::runtime_error("socket() failed");
    }
}

void    Server::__closeAllSocketlist(void)
{
    if(socketlist.size() > 0)
    {
        for (size_t i = 0; i < socketlist.size(); i++)
            close(socketlist[i]);
    }
}

void Server::__setReuseAddr(int &sockfd)
{
    int on=1;
    //Set up hotel for serve client this it set up for client use a seem door
    if (setsockopt(sockfd, SOL_SOCKET,  SO_REUSEADDR,(char *)&on, sizeof(on)) < 0)
    {
        close(sockfd);
        __closeAllSocketlist();
        throw std::runtime_error("setsockopt() failed");
    }
}

void Server::__getInformAndBind(int &sockfd, std::string &ip, std::string &port)
{
    // Set Up Rooms (Bind Socket): You inform the hotel staff (operating system) 
    // about the hotel's address (IP address) and room 
    // availability (port number) using bind().
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // Request IPv4 addresses
    hints.ai_socktype = SOCK_STREAM;  // Request TCP sockets

    int status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res);
    if (status != 0) {
        close(sockfd);
        __closeAllSocketlist();
        throw std::runtime_error("getaddrinfo() failed");
    }

    struct sockaddr_in server_addr;
    memcpy(&server_addr, res->ai_addr, res->ai_addrlen);  // Copy address info
    freeaddrinfo(res);  // Free memory allocated by getaddrinfo

    server_addr.sin_port = htons(std::atoi(port.c_str()));  // Convert port to network byte order
    // Bind the socket to the address and port
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        close(sockfd);
        __closeAllSocketlist();
        throw std::runtime_error("bind() failed");
    }
}
int Server::__createNewSocket(std::string ip ,std::string port)
{
            int sockfd;
            __getNewSocketfd(sockfd);
            __setReuseAddr(sockfd);
			__setNonBlocking(sockfd);
            __getInformAndBind(sockfd, ip, port);
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