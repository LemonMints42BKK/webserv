#include "Http.hpp"

#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <iostream>
#include <algorithm>

http::HttpV1::HttpV1(int socket, cfg::Configs *configs) 
	: Http(socket, configs), _stage(START_LINE), _request(NULL), _response(NULL)
{}

http::HttpV1::~HttpV1()
{
	if (_request) delete _request;
	if (_response) delete _response;
}

/* return true to close socket */
bool http::HttpV1::readSocket()
{
	char buffer[HTTP_BUFFER];
	int byte_read;

	while (true) {
		// byte_read = recv(_socket, buffer, HTTP_BUFFER - 1, 0);
		byte_read = recv(_socket, buffer, HTTP_BUFFER - 1, MSG_DONTWAIT);
		if (byte_read < 0) return (false);
		if (byte_read == 0) return (true);

		buffer[byte_read] = 0;
		_data.write(buffer, byte_read);
		
		// std::cout << "debug from http byte_read: " << byte_read << std::endl; 
		// debug
		// std::cout << _data.str() << std::endl;

		if(!parser()) return (true);
	}

	return (false);
}

bool http::HttpV1::writeSocket()
{
	return (true);
}

bool http::HttpV1::parser()
{

	if (_stage == START_LINE) {
		_request = new Request;
		_response = new Response;
		if (!parserFirstLine()) {
			delete _request;
			_request = NULL;
			delete _response;
			_response = NULL;
			_stage = START_LINE;
			return (false);
		}

		// debug 
		std::cout << _request->getMethod() << " " << _request->getLocation() << std::endl;
	}

	if (_stage == HEADER) {
		if (!parserHeader()) {
			delete _request;
			_request = NULL;
			delete _response;
			_response = NULL;
			_stage = START_LINE;
			return (false);
		}
	}

	if (_stage == BODY) {
		// if(!parserBody()) return true;
		parserBody();
	}

	if (_stage == ROUTER) {
		router();
	}

	if (_stage == RESPONSED) {
		delete _request;
		_request = NULL;
		delete _response;
		_response = NULL;
		_stage = START_LINE;
		return (false);
	}

	return (true);
}

bool http::HttpV1::parserFirstLine()
{
	std::string buffer;
	std::getline(_data, buffer);	
	std::istringstream line(buffer);

	// check method
	line >> buffer;
	if (line.fail()) {
		_stage = RESPONSED;
		return (_response->response(_socket, 400));
	} 
	_request->setMethod(buffer);

	// check location
	line >> buffer;
	if (line.fail()) {
		_stage = RESPONSED;
		return (_response->response(_socket, 400));
	} 
	_request->setLocation(buffer);

	_stage = HEADER;
	return (true);
}

bool http::HttpV1::parserHeader()
{
	while (true) {
		std::string buffer;
		std::getline(_data, buffer);

		if (!buffer.length() || buffer.c_str()[0] == '\r') 
		{
			//check if has content body
			if (_request->getHeader("Content-Type").length()) {
				// std::cout << "parserHeader should parserBody" << std::endl;
				_stage = BODY;

				std::string buffer;
				std::istringstream iss(_request->getHeader("Content-Type"));
				iss >> buffer;
				// std::cout << buffer << std::endl;
				iss >> buffer;
				// std::cout << buffer << std::endl;
				_boundary = "--" + buffer.substr(9) + "--\r";
			}
			else _stage = ROUTER;
			break ;
		}
		std::size_t colon = buffer.find(':');
		if (colon == std::string::npos) {
			_stage = RESPONSED;
			return (_response->response(_socket, 400));
		}
		std::string key = buffer.substr(0, colon);
		std::string value = buffer.substr(colon + 1);
		if (!key.length() || !value.length()) {
			_stage = RESPONSED;
			return (_response->response(_socket, 400));
		}

		key = trim(key);
		value = trim(value);
		_request->setHeader(key, value);

	}
	return (true);
}

bool http::HttpV1::parserBody()
{
	std::string buffer;
	// while (std::getline(_data, buffer))
	// {
		// std::cout << "parserBody: " << buffer << std::endl;
		// std::cout << "parserBody: " << _boundary << std::endl;
		std::stringstream tmp;
		tmp << _data.rdbuf();
		_body << tmp.str();
		while (std::getline(tmp, buffer))
		{
			std::cout << buffer << std::endl;
			if (buffer == _boundary) {
				// std::cout << tmp.str() << std::endl;
				std::cout << "found boundary" << std::endl;
				_stage = ROUTER;
				return (true);

			}
		}
	// }
	// send(_socket, "100 Continue", 13, 0);
	return (false);
}

bool http::HttpV1::router()
{
	std::string location = _request->getLocation();
	std::string host = _request->getHeader("Host");
	cfg::Location *loc = _configs->getLocation(host, location);
	if (!loc) 
		return (tryFiles());
	else if (loc->isCgi()) {
		_request->setCgiFile(loc->getCgiFile());
		_request->setCgiExe(loc->getCgiExe());
		return (cgi());
	}
	else if (loc->getLocation() == "/upload") {
		std::cout << _body.str() << std::endl;
		_stage = RESPONSED;
		return _response->response(_socket, 201);
	}
	else return (tryFiles());
}

time_t http::HttpV1::getTime()
{
	time_t timer;
    return time(&timer);
}

pid_t http::HttpV1::wait_Child(pid_t child_pid, int *status)
{
    pid_t exited_pid;
    time_t start_time = getTime();
    time_t post_time = start_time + 3;
    while ((exited_pid = waitpid(child_pid, status, WNOHANG)) == 0) {
      printf("Child process (pid: %d) hasn't exited yet...\n", child_pid);
      start_time = getTime();      
      if (post_time - start_time == 0)
        kill(child_pid, SIGKILL);
      sleep(1); // Check again after 1 second
    }
    return exited_pid;
}

void http::HttpV1::getExiteAndStatusForResponse(pid_t exited_pid, int status)
{
	if(exited_pid < 0) {
		std::cout << "CGI: failed" << std::endl;
		_stage = RESPONSED;
		_response->response(_socket, 500);
	}

	if (WIFEXITED(status)) {
		int exit_status = WEXITSTATUS(status);
		if (exit_status == 0) {
			std::cout << "CGI: success" << std::endl;
			_stage = RESPONSED;
			_response->response(_socket, 200, "./www/cgi.html", "text/html");
		} else {
			_stage = RESPONSED;
			std::cout << "CGI: failed" << std::endl;
			_response->response(_socket, 500);
		}
	} else {
		std::cout << "CGI: failed" << std::endl;
		_stage = RESPONSED;
		_response->response(_socket, 500);
	}
}

bool http::HttpV1::cgi()
{
	_stage = RESPONSED;
	pid_t pid = fork();
	if(pid == 0)
	{
		pid_t child_pid = fork();
		if (child_pid == 0) {
			char *envp[] = {NULL};
			// char *argv[3] = {"/usr/bin/python3", "./CgiFile/GenPage.py", NULL};
			char *argv[3];
			argv[0] = strdup("/usr/bin/python3");
			argv[1] = strdup("http/CgiFile/GenPage.py");
			argv[2] = NULL;
			if(execve(argv[0], argv, envp) == -1)
				exit(1);
			exit(0);
		} else {
			// Parent process
			int status;
			pid_t exited_pid = wait_Child(child_pid, &status);
			getExiteAndStatusForResponse(exited_pid, status);
		}
		exit(0);
	}	
	return (true);
	// return (_response->response(_socket, 200, "./www/cgi_inconstruction.html", "text/html"));
}

bool http::HttpV1::tryFiles()
{
	std::string location = _request->getLocation();
	std::string host = _request->getHeader("Host");

	std::string root;
	try {
		root = _configs->getRoot(host, location);
	}
	catch (std::exception const &e){
		_stage = RESPONSED;
		return (_response->response(_socket, 502));
	}

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
		std::vector<std::string>::const_iterator it = std::find(methods.begin(), methods.end(), _request->getMethod());
		if (it == methods.end()) {
			_stage = RESPONSED;
			return (_response->response(_socket, 405));
		}
	
		//check type

		std::string contentType = _configs->getTypes(file);

		//response here
		_stage = RESPONSED;
		return _response->response(_socket, 200, file, contentType);

		// return (true);
	}

	return (false);
}

/* utils */

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

std::string http::HttpV1::trim(std::string &str) const
{
	std::size_t start = str.find_first_not_of(" \t\n\r");
	if (start == std::string::npos) return ("");
	std::size_t end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}
