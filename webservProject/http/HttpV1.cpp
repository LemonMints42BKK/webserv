#include "Http.hpp"

#include <unistd.h>
#include <cstring>
#include <sys/socket.h>

#include <iostream>


http::HttpV1::HttpV1(int socket, cfg::Configs *configs) : Http(socket, configs)
{

}

http::HttpV1::~HttpV1()
{

}

/* return true to close socket */
bool http::HttpV1::readSocket()
{
	char buffer[HTTP_BUFFER];
	int byte_read;

	while (true) {
		byte_read = read(_socket, buffer, HTTP_BUFFER - 1);
		if (byte_read < 0) return (false);
		if (byte_read == 0) return (true);

		buffer[byte_read] = 0;
		_data.write(buffer, byte_read);

		if (endMessage(buffer)) {
			parserHeader();
		}
		// std::cout << "socket: " << _socket << std::endl << _data.str() << std::endl;
	}
	return (false);
}

bool http::HttpV1::endMessage(const char *buffer) const
{
	return (std::strstr(buffer, "\n\n") || std::strstr(buffer, "\r\n\r\n"));
}

void http::HttpV1::parserHeader()
{
	std::cout << "socket: " << _socket <<std::endl;
	// << _socket << std::endl << _data.str() << std::endl;
	// _method

	std::ostringstream oss;

	// std::string message = "Hello this message sent from PTP";
	// oss << "HTTP/1.1 200 OK" << std::endl
	// 	<< "Server: PTP" << std::endl
	// 	<< "Content-Type: text/plan;" << std::endl
	// 	<< "Content-Length: " << message.length() << std::endl
	// 	<< std::endl
	// 	<< message
	// 	<< std::endl
	// 	<< std::endl;
	// send(_socket, oss.str().c_str(), oss.str().length(), 0);

	std::ifstream file("www/index.html");
	// if (!file.is_open()) {
	// 	std::cerr << "file index not found" << std::endl;
	// 	oss << "HTTP/1.1 200 OK" << std::endl << std::endl;
	// 	write(_socket, oss.rdbuf(), oss.tellp());
	// 	return ;
	// }
 	//  std::streampos fileSize = file.tellg();
	if (file.is_open()) {
		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();
		file.seekg(0);
		// std::cout << "length: " << fileSize << std::endl;
		oss << "HTTP/1.1 200 OK" << std::endl;
		oss << "Server: PTP" << std::endl;
		oss << "Connect: Keep-Alive" << std::endl;
		oss << "Keep-Alive: timeout=5, max=1000" << std::endl;
		oss << "Content-Type: text/html" << std::endl;
		oss << "Content-Length: " << fileSize << std::endl;
		oss << std::endl;
		oss << file.rdbuf();
		
		// std::cout << oss.str() << std::endl;
		send(_socket, oss.str().c_str(), oss.str().length(), 0);
	}
	else {
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
}

