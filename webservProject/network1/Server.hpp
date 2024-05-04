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


namespace server
{
	#define MAX_EVENTS 10
	#define MYHTTP http::HttpV1
	
	class Server
	{
	private:
		cfg::Configs *_configs;
		bool _epoll_loop;
		int _epoll_fd;
		std::vector<int> _server_sockets;
		std::map<int, MYHTTP*> _http;
		int __create_tcp_server_socket(std::string const &interface, std::string const &port);
		void __close_all_server_sockets();
		void __accept_new_connection_request(int fd);
		void __epoll_create();
		void __epoll_loop();
		void __handle_event1(struct epoll_event &event);
		// void __handle_event2(struct epoll_event &event);
	public:
		Server(cfg::Configs *configs);
		~Server();
		void start_server();
		void stop_server();
	};
}

#endif