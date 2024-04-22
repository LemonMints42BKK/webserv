#include "Http.hpp"

#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/stat.h>

#include <iostream>
#include <algorithm>

http::HttpV1::HttpV1(int socket, cfg::Configs *configs) 
	: Http(socket, configs), _stage(METHOD){}

http::HttpV1::~HttpV1(){}

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
		
		// std::cout << "debug from http byte_read: " << byte_read << std::endl; 

		if(!parser()) return (true);
	}

	return (false);
}

bool http::HttpV1::parser()
{
	// std::cout << "socket: "<< _socket << ", stage: " << _stage  <<std::endl;

	if (_stage == METHOD) {
		if (!parserFirstLine()) return (false);
	}

	// std::cout << "socket: "<< _socket << ", stage: " << _stage  <<std::endl;

	if (_stage == HEADER) {
		if (!parserHeader()) return (false);
	}

	return (true);
}

// bool http::HttpV1::endMessage(const char *buffer) const
// {
// 	return (std::strstr(buffer, "\n\n") || std::strstr(buffer, "\r\n\r\n"));
// }

bool http::HttpV1::parserFirstLine()
{
	std::string buffer;
	std::getline(_data, buffer);

	// std::cout << "parserFirstLine: " << buffer << std::endl;
	
	std::istringstream line(buffer);

	// check method
	line >> buffer;
	if (line.fail()) {
		// response 400 bad request
		std::cout << "parserFirstLine 1" << std::endl;
		return (false);
	}
	// if (buffer != "GET" && buffer != "POST" && buffer != "DELETE" ) {
	// 	// response 405 method not allow
	// 	return (false);
	// }
	_request.setMethod(buffer);

	// check location
	line >> buffer;
	if (line.fail()) {
		// response 400 bad request
		std::cout << "parserFirstLine 2" << std::endl;
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

		// std::cout << "parserHeader: buffer: " << buffer.length() << std::endl;
		// if (buffer.length() == 1) {
		// 	std::cout << "parserHeader: buffer: " << int(buffer.c_str()[0]) << std::endl;
		// }

		if (!buffer.length() || buffer.c_str()[0] == '\r') {

			// std::cout << "parserHeader: end" << std::endl;
			// std::cout << _request;
			// check location root if not exist return 404
			if (!router()) {
				return _response.response(_socket, 404);
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
		if (!key.length() || !value.length()) {
			// response 400 bad request
			return (false);
		}
		// std::cout << "parserHeader: " << key << " " << value << std::endl;
		key = trim(key);
		value = trim(value);
		_request.setHeader(key, value);

		// std::cout << "parserHeader: " << key << " " << value << std::endl;
	}
	return (true);
}



std::string http::HttpV1::trim(std::string &str) const
{
	std::size_t start = str.find_first_not_of(" \t\n\r");
	if (start == std::string::npos) return ("");
	std::size_t end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}

bool http::HttpV1::router()
{
	std::string location = _request.getLocation();
	std::string host = _request.getHeader("Host");
	cfg::Location *loc = _configs->getLocation(host, location);
	if (!loc) 
		return (tryFiles());
	// else if (loc->getLocation() == "upload")
	else if (loc->isCgi()) {
		_request.setCgiFile(loc->getCgiFile());
		_request.setCgiExe(loc->getCgiExe());
		return (cgi());
	}
	else return false;
}

bool http::HttpV1::cgi()
{
	return (_response.response(_socket, 200, "./www/cgi_inconstruction.html", "text/html"));
}

bool http::HttpV1::tryFiles()
{
	std::string location = _request.getLocation();
	std::string host = _request.getHeader("Host");

	std::string root = _configs->getRoot(host, location);
	if (!root.length()) return (false);

	// std::cout << "tryFiles: root: " << root << std::endl;
	std::string file = root + location;

	if (fileExists(file)) {
		// std::cout << "tryFiles: found: " << file << std::endl;
		if (isDirectory(file)) {
			std::vector<std::string> indexs = _configs->getIndex(host, location);
			// std::cout << "tryFiles: indexs: " << indexs.size() << std::endl;
			if (!indexs.size())
				return (false);
			for(std::vector<std::string>::const_iterator it = indexs.begin();
				it != indexs.end(); it++)
			{
				// std::cout << "tryFiles: index: " << *it << std::endl;

				if (isFile(file + *it)) {
					// std::cout << "tryFiles: match index: " << file + *it << std::endl;
					file = file + *it;
					break;
				}
			}
		}
		if (!isFile(file)) {
			return (false);
		}

		//check allow method
		std::vector<std::string> methods = _configs->getAllow(host, location);
		std::vector<std::string>::const_iterator it = std::find(methods.begin(), methods.end(), _request.getMethod());
		if (it == methods.end())
			return (_response.response(_socket, 405));
	
		//check type

		std::string contentType = _configs->getTypes(file);

		//response here

		return _response.response(_socket, 200, file, contentType);

		// return (true);
	}

	return (false);
}

bool http::HttpV1::fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

bool http::HttpV1::isDirectory(const std::string& filename) {
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) == 0) {
        return S_ISDIR(buffer.st_mode);
    }
    return false;
}

bool http::HttpV1::isFile(const std::string& filename) {
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) == 0) {
        return S_ISREG(buffer.st_mode);
    }
    return false;
}
