#include "Server.hpp"



server::Server::Server(cfg::Configs *configs) : _configs(configs)
{

}

server::Server::~Server()
{
}

void server::Server::start_server()
{
	cfg::ListenPairs listens;
	_configs->getListenPairs(listens, _configs->begin(), _configs->end());

	for (cfg::ListenPairs::const_iterator it = listens.begin(); it != listens.end(); it++) {

		/* Get the socket server fd */
		int server_socket = __create_tcp_server_socket(it->first, it->second);
		if (server_socket == -1) {
			// close_all_server_sockets();
			throw std::runtime_error("failed on create_tcp_server_socket");
		}

		/* Make the socket non blocking, will not wait for connection indefinitely */ 
		fcntl(server_socket, F_SETFL, O_NONBLOCK);
		if (server_socket == -1) {
			close (server_socket);
			// close_all_server_sockets();
			throw std::runtime_error("failed on create_tcp_server_socket make non blocking");
		}

		_server_sockets.push_back(server_socket);
		std::cout << "listen on socket: " << server_socket << std::endl;
	}

	/* epoll */
	// struct epoll_event ev, events[MAX_EVENTS];

	/* Create epoll instance */
	_epoll_fd = epoll_create (MAX_EVENTS);
	if (_epoll_fd == -1) {
		throw std::runtime_error ("epoll_create");
	}
	_server_sockets.push_back(_epoll_fd);
	
	for (std::size_t i = 0; i < _server_sockets.size() - 1; i++) {
		
		_ev.data.fd = _server_sockets[i];

		/* Interested in read's events using edge triggered mode */
		_ev.events = EPOLLIN | EPOLLET ;

		/* Allow epoll to monitor the server_fd socket */
		if (epoll_ctl (_epoll_fd, EPOLL_CTL_ADD, _server_sockets[i], &_ev) == -1) {
			throw std::runtime_error ("epoll_ctl");
		}
	}

	while (1) {
		/* Returns only sockets for which there are events */
		int nfds = epoll_wait(_epoll_fd, _events, MAX_EVENTS, -1);

		if (nfds == -1) {
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}

		/* Iterate over sockets only having events */
		for (int i = 0; i < nfds; i++) {
			int fd = _events[i].data.fd;
			
			// std:: cout << "debug: " << events[i].events << std::endl;

			if (std::find(_server_sockets.begin(), _server_sockets.end(), fd) != _server_sockets.end()) {
				/* New connection request received */
				__accept_new_connection_request(fd);
			}
			
			else 
			{
				if ((_events[i].events & EPOLLERR) || (_events[i].events & EPOLLRDHUP)) {

				/* Client connection closed */
					std::cout << "Close by socket: " << fd << std::endl;
					delete _http[_events[i].data.fd];
					_http.erase(_events[i].data.fd);
					close(fd);
				}
				else if (_events[i].events & EPOLLIN) {
					/* Received data on an existing client socket */
					std::cout << "Connect by socket: " << fd << std::endl;
					// recv_and_forward_message(fd);
					_http[_events[i].data.fd]->readSocket();
				}
			}
		}
	}
}

void server::Server::__accept_new_connection_request(int fd) {
    struct sockaddr_in new_addr;
    int addrlen = sizeof(struct sockaddr_in);

    while (1) {
        /* Accept new connections */
        int conn_sock = accept(fd, (struct sockaddr*)&new_addr, 
                          (socklen_t*)&addrlen);
        
        if (conn_sock == -1) {
            /* We have processed all incoming connections. */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            }
            else {
                perror ("accept");
                break;
            }
        }

        /* Make the new connection non blocking */
        fcntl(conn_sock, F_SETFL, O_NONBLOCK);

        /* Monitor new connection for read events in edge triggered mode */
        _ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        _ev.data.fd = conn_sock;

        /* Allow epoll to monitor new connection */
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, conn_sock, &_ev) == -1) {
            perror("epoll_ctl: conn_sock");
            break;
        }

		/* Create Http */
		_http[conn_sock] = new MYHTTP(conn_sock, _configs);
    }
}

void server::Server::stop_server()
{
	__close_all_server_sockets();
	for(std::map<int, MYHTTP*>::const_iterator it = _http.begin(); it != _http.end(); it++) {
		std::cout << "http on socket: " << it->first << " was delete" << std::endl;
		delete it->second;
	}
}

void server::Server::__close_all_server_sockets()
{
	for(std::vector<int>::const_iterator it = _server_sockets.begin();
		it != _server_sockets.end(); it++) {
		std::cout << "server_socket: " << *it << " closed" << std::endl;
		close(*it);
	}
}

int server::Server::__create_tcp_server_socket(std::string const &interface, std::string const &port)
{
	const int opt = 1;

	/* Step1: create a TCP socket */
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1) {
		perror("Could not create socket");
		return (-1);
	}

	// printf("Created a socket with fd: %d\n", fd);

	/* Step2: set socket options */
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == -1) { 

		perror("Could not set socket options");
		close(fd);
		return (-1);
	} 

	/* Initialize the socket address structure */
	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  // Request IPv4 addresses
	hints.ai_socktype = SOCK_STREAM;  // Request TCP sockets

	int status = getaddrinfo(interface.c_str(), port.c_str(), &hints, &res);
	if (status != 0) {
		close(fd);
		perror("getaddrinfo() failed");
		return (-1);
	}

	struct sockaddr_in server_addr;
	memcpy(&server_addr, res->ai_addr, res->ai_addrlen);  // Copy address info
	freeaddrinfo(res);  // Free memory allocated by getaddrinfo

	server_addr.sin_port = htons(std::atoi(port.c_str()));  // Convert port to network byte order

    // Step3: Bind the socket to the address and port
    if (bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        close(fd);
        perror("bind() failed");
		return (-1);
    }

	/* Step4: listen for incoming connections */
	/* 
		The socket will allow a maximum of 1000 clients to be queued before 
		refusing connection requests.
	*/
	if (listen(fd, 1000) == -1) {
		perror("Could not listen on socket");
		close(fd);
		return (-1);
	}

	return fd;
}
