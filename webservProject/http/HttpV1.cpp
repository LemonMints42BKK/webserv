#include "Http.hpp"

#include <unistd.h>
#include <cstring>
#include <sys/socket.h>

#include <iostream>


http::HttpV1::HttpV1(int socket, cfg::Configs *configs) 
	: Http(socket, configs), _stage(METHOD)
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

		if(!parser()) return (true);
	}

	return (false);
}

bool http::HttpV1::endMessage(const char *buffer) const
{
	return (std::strstr(buffer, "\n\n") || std::strstr(buffer, "\r\n\r\n"));
}

bool http::HttpV1::parserFirstLine()
{
	std::string buffer;
	std::getline(_data, buffer);

	std::istringstream line(buffer);

	// check method
	line >> buffer;
	if (line.fail()) {
		// response 400 bad request
		return (false);
	}
	if (buffer != "GET" && buffer != "POST" && buffer != "DELETE" ) {
		// response 405 method not allow
		return (false);
	}
	_request.setMethod(buffer);

	// check location
	line >> buffer;
	if (line.fail()) {
		// response 400 bad request
		return (false);
	}
	_request.setLocation(buffer);

	_stage = HEADER;
	return (true);
}

bool http::HttpV1::parserHeader()
{
	while (true) {
		std::string buffer;
		std::getline(_data, buffer);
		if (!buffer.length()) {

			// check location root if not exist return 404
			if (!tryFiles()) {
				// return 404
			}
			
			//check if has content body
			if (_request.getHeader("Content-Type").length()) {
				_stage = BODY;
			}

			// make response


			return (true);
		}
		std::size_t colon = buffer.find(':');
		if (colon == std::string::npos) {
			// response 400 bad request
			return (false);
		}
		std::string key = buffer.substr(0, colon);
		std::string value = buffer.substr(colon + 1);
		if (!key.length() || value.length()) {
			// response 400 bad request
			return (false);
		}
		key = trim(key);
		value = trim(value);
		_request.setHeader(key, value);
	}
	return (true);
}

bool http::HttpV1::parser()
{
	std::cout << "socket: " << _socket <<std::endl;

	if (_stage == METHOD) {
		if (!parserFirstLine()) return (false);
	}

	if (_stage == HEADER) {
		if (!parserHeader()) return (false);
	}

	// << _socket << std::endl << _data.str() << std::endl;
	// _method

	// std::ostringstream oss;

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

	// std::ifstream file("www/index.html");
	// if (!file.is_open()) {
	// 	std::cerr << "file index not found" << std::endl;
	// 	oss << "HTTP/1.1 200 OK" << std::endl << std::endl;
	// 	write(_socket, oss.rdbuf(), oss.tellp());
	// 	return ;
	// }
 	//  std::streampos fileSize = file.tellg();
	// if (file.is_open()) {
	// 	file.seekg(0, std::ios::end);
	// 	std::streampos fileSize = file.tellg();
	// 	file.seekg(0);
	// 	// std::cout << "length: " << fileSize << std::endl;
	// 	oss << "HTTP/1.1 200 OK" << std::endl;
	// 	oss << "Server: PTP" << std::endl;
	// 	oss << "Connect: Keep-Alive" << std::endl;
	// 	oss << "Keep-Alive: timeout=5, max=1000" << std::endl;
	// 	oss << "Content-Type: text/html" << std::endl;
	// 	oss << "Content-Length: " << fileSize << std::endl;
	// 	oss << std::endl;
	// 	oss << file.rdbuf();
		
	// 	// std::cout << oss.str() << std::endl;
	// 	send(_socket, oss.str().c_str(), oss.str().length(), 0);
	// }
	// else {
	// 	std::string message = "Hello this message sent from PTP";
	// 	oss << "HTTP/1.1 200 OK" << std::endl
	// 		<< "Server: PTP" << std::endl
	// 		<< "Content-Type: text/plan;" << std::endl
	// 		<< "Content-Length: " << message.length() << std::endl
	// 		<< std::endl
	// 		<< message
	// 		<< std::endl
	// 		<< std::endl;
	// 	send(_socket, oss.str().c_str(), oss.str().length(), 0);
	// }

	return (true);
}

std::string http::HttpV1::trim(std::string &str) const
{
	std::size_t start = str.find_first_not_of(" \t\n\r");
	if (start == std::string::npos) return ("");
	std::size_t end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}

bool http::HttpV1::tryFiles()
{
	std::string location = _request.getLocation();


	return (false);
}

bool http::HttpV1::errorPage(int status)
{
	switch (status)
	{
	case 404:
		/* code */
		break;
	
	default:
		break;
	}
	return (true);
}