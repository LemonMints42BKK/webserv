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
#include <time.h>
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

using namespace server;
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

void Server::start_server()
{
	__init_config();
	__getInputAndCreateListSocket();
	__setUpMoniterSocket();
	__runMoniter();
}

void Server::stop_server()
{
	__setCleanupSocket();
}

void Server::__getInputAndCreateListSocket(void)
{
	cfg::Listens listens;
	_configs->getListen_getIp(listens, _configs->begin(), _configs->end());
	for (Listen it = listens.begin(); it != listens.end(); ++it) {
		const std::string& key = it->first;
		const std::vector<std::string>& value = it->second;
		for (unsigned int i = 0; i < value.size(); ++i) {
			socketlist.push_back(__createNewSocket(key, value.at(i)));
		}
	}
}

void Server::__setUpMoniterSocket(void)
{
	// std::cout << "Set Up Moniter Socket" << std::endl;
	FD_ZERO(&_master_set);
	FD_ZERO(&_working_set);
	for (size_t i = 0; i < socketlist.size(); ++i) {
		FD_SET(socketlist[i], &_master_set);
	}
	_max_sd = socketlist.back();
}	

void Server::__runMoniter(void)
{
	struct timeval timeout;
	std::memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec  = 5;
	timeout.tv_usec = 0;
	int rc ;
	while(true)
	{
		// struct timeval timeout = {0, 5000000}; // 0.5 seconds
		// print_fd_set(&_master_set);
		std::memcpy(&_working_set, &_master_set, sizeof(_master_set));
		// printf("  Waiting on select()...\n");
		rc = select(_max_sd + 1, &_working_set, NULL, NULL,  &timeout);
		if (rc < 0)
		{
			perror("select() failed");
			break;
		}
		if (rc == 0)
		{
			// printf("  select() timed out.  Check conn.\n");
			__checkClientTimeOut();
			std::memset(&_working_set, 0, sizeof(_working_set));
			timeout.tv_sec  = 5;
			timeout.tv_usec = 0;
			continue;
		}
		__loopCheckFd_workingSet();
	} 
}

void Server::__checkClientTimeOut()
{
	for (size_t i = 0; i <= static_cast<size_t>(_max_sd); ++i)
	{
		if (FD_ISSET(i, &_master_set))
		{
			// std::cout << "Time " << _time[i] << std::endl;
			if (!__checkIsSocketListen(i) && _time[i] + 3 < __getTime() )
			{
				// std::cout << RED << "Time out " << i << RESET << std::endl;
				_close_conn = true;
				__handle_close_conn(i);
			}
		}
	}
}

bool Server::__checkIsSocketListen(size_t &socket)
{
	return std::find(socketlist.begin(), socketlist.end(), socket) != socketlist.end();
}

void Server::__loopCheckFd_workingSet(void)
{
	int des_ready = 1;
	_close_conn = false;
	for (size_t i = 0; i <= static_cast<size_t>(_max_sd) && des_ready > 0; ++i)
	{
		// bool is_socket_listen = std::find(socketlist.begin(), socketlist.end(), i) != socketlist.end();
		if (FD_ISSET(i, &_working_set))
		{
			des_ready -= 1;
			if (__checkIsSocketListen(i))
				__requestFromClient(static_cast<int>(i)); // Accept new client
			else
			{
				//__start_http(i); // replace http.readSocket() here
				_close_conn = _http[i]->readSocket();
				__handle_close_conn(i);
			}
		}
	}/* End of loop through selectable descriptors */
}

void Server::__handle_close_conn(size_t socketfd)
{
	if (_close_conn)
	{
		std::cout << RED << "Close connection " << socketfd << RESET << std::endl;
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


void Server::__setCleanupSocket(void)
{
	for (int i=0; i <= _max_sd; ++i)
	{
		if (FD_ISSET(i, &_master_set))
		{
			close(i);
			FD_CLR(i, &_master_set);
			if(_http[i] != NULL)
				delete _http[i];
		}
	}
	print_fd_set(&_master_set);
	std::cout << "bye bye" << std::endl;
}

void Server::__requestFromClient(int socket)
{
	int new_sd;
	do
	{
		new_sd = accept(socket, NULL, NULL);
		// InfoClient(new_sd);
		if (new_sd < 0)
			break;
		__setNonBlocking(new_sd);
		std::cout << YELLOW << "New incoming connection " << new_sd << RESET <<std::endl;
		// printf("New incoming connection %d\n", new_sd);
		_http[new_sd] = new http::HttpV1(new_sd, _configs);
		_time[new_sd] = __getTime();
		// printf("Time In %.f\n", _time[new_sd]);
		// printf("first client %lu\n", 3 + socketlist.size());
		FD_SET(new_sd, &_master_set);
		if (new_sd > _max_sd)
			_max_sd = new_sd;
		// printf("New max_sd %d\n", _max_sd);
	} while (new_sd != -1);
}

void Server::__setNonBlocking(int socket)
{
	int result = fcntl(socket, F_SETFL, O_NONBLOCK);
	if (result == -1) {
		throw std::runtime_error("fcntl() failed");
	}
}

time_t Server::__getTime()
{
	time_t timer;
    return time(&timer);
}



// int InfoClient(int socketfd)
// {
// 	int sockfd = socketfd;
//     struct sockaddr_in addr;
//     socklen_t addr_len = sizeof(addr);

//     // Assume sockfd is already initialized and connected

//     // Get the local address and port of the socket
//     if (getsockname(sockfd, (struct sockaddr *)&addr, &addr_len) == -1) {
//         perror("getsockname() failed");
//         return 1;
//     }

//     // Print the local address and port
//     char ip[INET_ADDRSTRLEN];
//     printf("Local IP: %s\n", inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN));
//     printf("Local Port: %d\n", ntohs(addr.sin_port));
// 	return 0;

// }

// bool Server::__getCheckMaster_AllZero(void)
// {
// 	for (size_t i = 0; i <= static_cast<size_t>(_max_sd); ++i)
// 	{
// 		if (FD_ISSET(i, &_master_set))
// 			return false;
// 	}
// 	return true;
// }

// void Server::__CloseSocketFdListenInMasterSet(void)
// {
// 	for (size_t i = 0; i < socketlist.size(); ++i)
// 	{
// 		close(socketlist[i]);
// 		FD_CLR(socketlist[i], &_master_set);
// 	}	
// }