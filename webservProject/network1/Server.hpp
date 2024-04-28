#ifndef SERVER_HPP
#define SERVER_HPP

#include "Configs.hpp"
#include "Http.hpp"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#define MAX_EVENTS 10
#define MYHTTP http::HttpV2

namespace server
{
	class Server
	{
	private:
		cfg::Configs *_configs;
		std::vector<int> _server_sockets;
		std::map<int, MYHTTP*> _http;
		int _epoll_fd;
		struct epoll_event _ev;
		struct epoll_event _events[MAX_EVENTS];
		int __create_tcp_server_socket(std::string const &interface, std::string const &port);
		void __close_all_server_sockets();
		void __accept_new_connection_request(int fd);
	public:
		Server(cfg::Configs *configs);
		~Server();
		void start_server();
		void stop_server();
	};
}

#endif