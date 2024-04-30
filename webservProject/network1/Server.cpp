#include "Server.hpp"



server::Server::Server(cfg::Configs *configs) : _configs(configs), _epoll_loop(true)
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
			throw std::runtime_error("failed on create_tcp_server_socket");
		}

		/* Make the socket non blocking, will not wait for connection indefinitely */ 
		// fcntl(server_socket, F_SETFL, O_NONBLOCK);
		// if (server_socket == -1) {
		// 	close (server_socket);
		// 	throw std::runtime_error("failed on create_tcp_server_socket make non blocking");
		// }

		_server_sockets.push_back(server_socket);
		std::cout << "listen on socket: " << server_socket << std::endl;
	}

	/* epoll */
	__epoll_create();
	__epoll_loop();
}

void server::Server::__epoll_create()
{
	_epoll_fd = epoll_create (MAX_EVENTS);
	if (_epoll_fd == -1) {
		throw std::runtime_error ("epoll_create");
	}

	// _server_sockets.push_back(_epoll_fd);
	
	for (std::vector<int>::const_iterator it = _server_sockets.begin(); it < _server_sockets.end(); it++) {
		
		struct epoll_event ev;
		ev.data.fd = *it;

		/* Interested in read's events using edge triggered mode */
		ev.events = EPOLLIN | EPOLLET ;

		/* Allow epoll to monitor the server_fd socket */
		if (epoll_ctl (_epoll_fd, EPOLL_CTL_ADD, *it, &ev) == -1) {
			throw std::runtime_error ("epoll_ctl");
		}
	}
}

void server::Server::__epoll_loop()
{
	struct epoll_event events[MAX_EVENTS];
	while (_epoll_loop) {
		/* Returns only sockets for which there are events */
		int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);

		// debug
		// std::cout << "event come in: " << nfds << std::endl;

		if (nfds == -1) {
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}

		/* Iterate over sockets only having events */
		for (int i = 0; i < nfds; i++) {

			struct epoll_event &event = events[i];

			if (std::find(_server_sockets.begin(), _server_sockets.end(), event.data.fd) != _server_sockets.end()
				&& event.events & EPOLLIN) {
				/* New connection request received */
				__accept_new_connection_request(event.data.fd);
				continue;
			}
			__handle_event1(event);		
		}
	}
}

/* for Http1 */
void server::Server::__handle_event1(struct epoll_event &event)
{
	MYHTTP *http = (MYHTTP *) event.data.ptr;
	if (!http) throw std::runtime_error("can not cast http");

	if (event.events & EPOLLERR || event.events & EPOLLRDHUP) {

		//debug
		std::cout << "Close by socket: " << http->getSocket() << std::endl;

		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, http->getSocket(), NULL);
		close (http->getSocket());
		delete http;
	}

	else if (event.events & EPOLLIN) {

		/* Received data on an existing client socket */
		http->readSocket();
	}
}

/* for Http2 */
void server::Server::__handle_event2(struct epoll_event &event)
{

	MYHTTP *http = (MYHTTP *) event.data.ptr;
	if (!http) throw std::runtime_error("can not cast http");

	if (event.events & EPOLLERR || event.events & EPOLLRDHUP) {

		//debug
		// std::cout << "Close by socket: " << http->getSocket() << std::endl;

		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, http->getSocket(), NULL);
		close (http->getSocket());
		delete http;
	}

	else if (event.events & EPOLLIN) {

		/* Received data on an existing client socket */
		http->readSocket();
		
		//debug
		// std::cout << "Connect by socket: " << http->getSocket() << " EPOLLIN" << std::endl;
		// std::cout << "read success set EPOLLOUT" << std::endl;

		// change event;
		event.events = EPOLLOUT | EPOLLRDHUP | EPOLLET;
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, http->getSocket(), &event) == -1) {
			close(http->getSocket());
			delete http;
			perror("epoll_ctl EPOLL_CTL_MOD: EPOLLIN");
		}
	}

	else if (event.events & EPOLLOUT) {

		/* send data on an existing client socket */
		http->writeSocket();

		// debug
		// std::cout << "Connect by socket: " << http->getSocket() << " EPOLLOUT" << std::endl;

		// close socket
		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, http->getSocket(), NULL);
		close (http->getSocket());
		delete http;
	}
}

void server::Server::__accept_new_connection_request(int fd) {
    struct sockaddr_in new_addr;
    int addrlen = sizeof(struct sockaddr_in);

    // while (1) {
        /* Accept new connections */
        int conn_sock = accept(fd, (struct sockaddr*)&new_addr, 
                          (socklen_t*)&addrlen);
        
        if (conn_sock == -1) {
            /* We have processed all incoming connections. */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                return;
            }
            else {
                perror ("accept");
                return;
            }
        }

        /* Make the new connection non blocking */
        // fcntl(conn_sock, F_SETFL, O_NONBLOCK);

        /* Monitor new connection for read events in edge triggered mode */
		struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        ev.data.fd = conn_sock;
		ev.data.ptr = new MYHTTP(conn_sock, _configs); 

        /* Allow epoll to monitor new connection */
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
			close(conn_sock);
            perror("epoll_ctl: conn_sock");
            return;
        }

		// debug
		std::cout << "New client connect on socket: " << conn_sock << std::endl;

}

void server::Server::stop_server()
{
	_epoll_loop = false;
	__close_all_server_sockets();
}

void server::Server::__close_all_server_sockets()
{
	for(std::vector<int>::const_iterator it = _server_sockets.begin();
		it != _server_sockets.end(); it++) {
		std::cout << "server_socket: " << *it << " closed" << std::endl;
		close(*it);
	}
	close (_epoll_fd);
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
