#include "Http.hpp"

#include <unistd.h>
#include <sys/socket.h>

http::ResponseV2::ResponseV2()
{
	_status[200] = "OK";
	_status[201] = "Created";
	_status[400] = "Bad Request";
	_status[404] = "Not Found";
	_status[405] = "Method Not Allowed";
	_status[502] = "Bad Gateway";
}

http::ResponseV2::~ResponseV2()
{}

void http::ResponseV2::setStatusLine(int status)
{
	std::ostringstream oss;
	oss << "HTTP/1.1" << " " << status << " " << _status[status];
	_status_line = oss.str();
}

void http::ResponseV2::setContent(std::string const &file)
{
	_content = file;
}

/*
    response with file
	the file must have extension
	the file must pass access function
*/
bool http::ResponseV2::response(int socket, int status, std::string const &filename, 
	std::string const &content_type)
{
    (void) socket;
	std::ifstream file(filename.c_str(), std::ios::binary);
	// std::stringstream ss;
	file.seekg(0, std::ios::end);
	_response << file.tellg();
	setStatusLine(status);
	_header["Content-Type"] = content_type;
	_header["Content-Length"] = _response.str();
	_header["Connection"] = "Keep-Alive";
	_header["Keep-Alive"] = "timeout=5, max=20";
	file.seekg(0);

	_response.str("");
	_response << _status_line << std::endl;
	std::map<std::string, std::string>::const_iterator it = _header.begin();
	for(; it != _header.end(); it++) {
		_response << it->first << ": " << it->second << std::endl;
	}
	_response << std::endl;
	_response << file.rdbuf();
	_response << std::endl;
	// write(socket, ss.str().c_str(), ss.tellp());
	// write(1, ss.str().c_str(), ss.tellp());

	return (true);
}

/*
    response with only status
*/
bool http::ResponseV2::response(int socket, int status)
{
	(void) socket;
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

	// std::stringstream ss;
	setStatusLine(status);
	_response << _status_line << std::endl;
	_response << "Connection: Keep-Alive" << std::endl;
	_response << "Keep-Alive: timeout=" << 5 << ", max=" << 20 << std::endl;
	_response << "Content-Type: text/html" << std::endl;
	_response << "Content-Length: " << content.tellp() << std::endl;
	_response << std::endl;
	_response << content.str();
	_response << std::endl;

	// write(socket, ss.str().c_str(), ss.tellp());
	return (true);
}

bool http::ResponseV2::writeSocket(int socket)
{
	// std::cout << "response write socket: " << socket << std::endl;
    // std::cout << _response.str() << std::endl;
    int bytez = send(socket, _response.str().c_str(), _response.tellp(), MSG_DONTWAIT);
    //debug 
    return bytez == -1 ? false : true;
}
