#include "Http.hpp"

#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <sstream>
#include <ctime>
http::Httptest::Httptest(int socket, cfg::Configs *configs) : Http(socket, configs)
{
    (void) _configs;
    (void) _socket;
};

http::Httptest::~Httptest()
{
    std::cout << "Httptest destructor called" << std::endl;
}

void http::Httptest::sendResponse()
{
    std::stringstream oss;
	std::string message = "Hello this message sent from PTP";
	oss << "HTTP/1.1 200 OK" << std::endl
		<< "Server: PTP" << std::endl
		<< "Content-Type: text/plan;" << std::endl
		<< "Content-Length: " << message.length() << std::endl
		<< std::endl
		<< message
		<< std::endl
		<< std::endl;
	send(_socket, oss.str().c_str(), oss.str().length(), 0);

}
bool http::Httptest::readSocket()
{
    std::cout << "Httptest readSocker called" << std::endl;
	int rc;
	char buffer[65000];
	// std::string request_line;
	// std::string temp;
//    __setBlocking(i);

// _http[socketfd].read_socket;
// while will replace http::read_Socket
    while (true)
    {
            // setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, NULL, 0);
            rc = recv(_socket, buffer, sizeof(buffer), 0);
            std::cout << "HTTP: " << rc << std::endl;
            if (rc < 0)
                return false;
            if (rc == 0)
            {
                printf("  Connection closed\n");
                return true;
            }
            _data.write(buffer, rc);
            std::cout << "Http length total:" << _data.tellp() << std::endl;
            sendResponse();
            // _http[_socket]->incrementTotalBytes(len);
            // _http[_socket]->sendResponse();
            // sendResponse();
    }
    return false;
}