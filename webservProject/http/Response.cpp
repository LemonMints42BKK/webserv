#include "Http.hpp"

#include <unistd.h>

http::Response::Response()
{
	_status[200] = "OK";
	_status[201] = "Created";
	_status[400] = "Bad Request";
	_status[404] = "Not Found";
	_status[405] = "Method Not Allowed";
	_status[502] = "Bad Gateway";
}

http::Response::~Response()
{}

void http::Response::setStatusLine(int status)
{
	std::ostringstream oss;
	oss << "HTTP/1.1" << " " << status << " " << _status[status];
	_status_line = oss.str();
}

void http::Response::setContent(std::string const &file)
{
	_content = file;
}

/*
    response with file
	the file must have extension
	the file must pass access function
*/
bool http::Response::response(int socket, int status, std::string const &filename, 
	std::string const &content_type)
{
	std::ifstream file(filename.c_str(), std::ios::binary);
	std::stringstream ss;
	file.seekg(0, std::ios::end);
	ss << file.tellg();
	setStatusLine(status);
	_header["Content-Type"] = content_type;
	_header["Content-Length"] = ss.str();
	// _header["Connection"] = "keep-alive";
	// _header["Keep-Alive"] = "timeout=5, max=20";
	file.seekg(0);

	ss.str("");
	ss << _status_line << std::endl;
	std::map<std::string, std::string>::const_iterator it = _header.begin();
	for(; it != _header.end(); it++) {
		ss << it->first << ": " << it->second << std::endl;
	}
	ss << std::endl;
	ss << file.rdbuf();
	ss << std::endl;
	write(socket, ss.str().c_str(), ss.tellp());
	// write(1, ss.str().c_str(), ss.tellp());

	return (true);
}

/*
    response with only status
*/
bool http::Response::response(int socket, int status)
{
	
	std::stringstream content;
	content << "<!DOCTYPE html>" << std::endl;
	content << "<html>" << std::endl;
	content << "<head>" << std::endl;
	content << "<title>" << status << " " << _status[status] << "</title>" << std::endl;
	content << "</head>" << std::endl;
	content << "<body>" << std::endl;
	content << "<h1>" << status << " " << _status[status] << "</h1>"  << std::endl;
	content << "</body>" << std::endl;
	content << "</html>" << std::endl;
	content << std::endl;

	std::stringstream ss;
	setStatusLine(status);
	ss << _status_line << std::endl;
	// ss << "Connection: keep-alive" << std::endl;
	// ss << "Keep-Alive: timeout=" << 5 << ", max=" << 20 << std::endl;
	ss << "Content-Type: text/html" << std::endl;
	ss << "Content-Length: " << content.tellp() << std::endl;
	ss << std::endl;
	ss << content.str();
	ss << std::endl;

	write(socket, ss.str().c_str(), ss.tellp());
	return (true);
}
