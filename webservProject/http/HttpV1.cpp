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

#define GREEN   "\033[32m"
#define RESET   "\033[0m"

bool http::HttpV1::parser()
{

	if (_stage == START_LINE) {
		_request = new Request;
		_response = new Response;
		if (!parserFirstLine()) {
			// delete _request;
			// _request = NULL;
			// delete _response;
			// _response = NULL;
			// _stage = START_LINE;
			// return (false);
			_stage = RESPONSED;
		}

		// debug 
		std::cout << GREEN << _request->getMethod() << " " << _request->getLocation() << RESET << std::endl;
	}

	if (_stage == HEADER) {
		if (!parserHeader()) {
			// delete _request;
			// _request = NULL;
			// delete _response;
			// _response = NULL;
			// _stage = START_LINE;
			// return (false);
			_stage = RESPONSED;
		}
	}

	if (_stage == BODY) {
		// if(!parserBody()) return true;
		parserBody();
	}

	if (_stage == ROUTER) {
		if (!router()) {
			_stage = RESPONSED;
			_response->response(_socket, 404);
		}
	}

	if (_stage == RESPONSED) {
		delete _request;
		_request = NULL;
		delete _response;
		_response = NULL;
		_stage = START_LINE;
		_data.str(std::string());
		_body.str(std::string());
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
				std::string buffer;
				std::istringstream iss(_request->getHeader("Content-Type"));
				iss >> buffer;
				if (buffer != "multipart/form-data;") {
					_stage = RESPONSED;
					return (_response->response(_socket, 422));
				}
				iss >> buffer;
				_boundary = "--" + buffer.substr(9) + "--\r";
				
				// check length of content
				int cmb = _configs->getClientMaxBody(_request->getHeader("Host"));
				if (cmb > 0 && cmb < std::atoi(_request->getHeader("Content-Length").c_str()))
				{
					_stage = RESPONSED;
					return (_response->response(_socket, 413));
				}
				_stage = BODY;
			}	
			else _stage = ROUTER;

			// set default error page
			_response->set_default_err_page(_configs->getErrorPage(_request->getHeader("Host")));
			// std::cout << "defalut error page: " << _configs->getErrorPage(_request->getHeader("Host")) << std::endl;
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
	std::stringstream tmp;
	tmp << _data.rdbuf();
	_body << tmp.str();
	while (std::getline(tmp, buffer))
	{
		if (buffer == _boundary) {
			std::cout << "found boundary" << std::endl;
			_stage = ROUTER;
			return (true);
		}
	}
	return (false);
}


bool http::HttpV1::router()
{
	std::string location = _request->getLocation();
	std::string host = _request->getHeader("Host");
	cfg::Location *loc = _configs->getLocation(host, location);
	//check allow method
	std::vector<std::string> methods = _configs->getAllow(host, location);
	std::vector<std::string>::const_iterator it = std::find(methods.begin(), methods.end(), _request->getMethod());
	if (it == methods.end()) {
		_stage = RESPONSED;
		return (_response->response(_socket, 405));
	}

	if (!loc) 
		return (tryFiles());
	else if (loc->isCgi()) {
		_request->setCgiFile(loc->getCgiFile());
		_request->setCgiExe(loc->getCgiExe());
		return (cgi());
	}
	else if (loc->getLocation() == "/upload") {

		std::string method = _request->getMethod().c_str();
		method = method.substr(0, method.find(' '));
		if(method != "POST") {
			_response->response(_socket, 405);
		}
		_request->setMethod("POST");
		cgiUpload();
		_stage = RESPONSED;
		return true;
	}
	else if (loc->getLocation() == "/delete")
	{
		std::cout << "delete" << std::endl;
		std::string method = _request->getMethod().c_str();
		method = method.substr(0, method.find(' '));
		if(method != "DELETE") {
			_response->response(_socket, 405);
		}
		_request->setMethod("DELETE");
		cgiDelete();
		_stage = RESPONSED;
		return true;
	}
	else if (loc->getRedirect().length()) 
	{
		std::cout << "redirect=" << loc->getRedirect() << std::endl;
		_stage = RESPONSED;
		return _response->response_redirect(_socket, 307, loc->getRedirect());
	}
	else return (tryFiles());
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
			std::vector<std::string> indexs;
			try {
				indexs = _configs->getIndex(host, location);
			}
			catch (std::exception const &e){
				return (false);
			}
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
		// std::vector<std::string> methods = _configs->getAllow(host, location);
		// std::vector<std::string>::const_iterator it = std::find(methods.begin(), methods.end(), _request->getMethod());
		// if (it == methods.end()) {
		// 	_stage = RESPONSED;
		// 	return (_response->response(_socket, 405));
		// }
	
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
