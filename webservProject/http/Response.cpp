#include "Http.hpp"

#include <unistd.h>
#include <sys/socket.h>

http::Response::Response()
{
	_status[200] = "OK";
	_status[201] = "Created";
	_status[307] = "Temporary Redirect";
	_status[400] = "Bad Request";
	_status[404] = "Not Found";
	_status[405] = "Method Not Allowed";
	_status[413] = "Content Too Large";
	_status[422] = "Unprocessable Content";
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
	_header["Connection"] = "Keep-Alive";
	_header["Keep-Alive"] = "timeout=5, max=20";
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
	send(socket, ss.str().c_str(), ss.tellp(), MSG_DONTWAIT);
	// write(1, ss.str().c_str(), ss.tellp());

	return (true);
}

/*
    response with only status
*/
bool http::Response::response(int socket, int status)
{
	// std::cout << _default_err_page << std::endl;
	if (status == 404 && _default_err_page.length())
	{
		return response(socket, 404, _default_err_page, "text/html");
	}
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
	ss << "Connection: Keep-Alive" << std::endl;
	ss << "Keep-Alive: timeout=" << 5 << ", max=" << 20 << std::endl;
	ss << "Content-Type: text/html" << std::endl;
	ss << "Content-Length: " << content.tellp() << std::endl;
	ss << std::endl;
	ss << content.str();
	ss << std::endl;

	send(socket, ss.str().c_str(), ss.tellp(), MSG_DONTWAIT);
	return (true);
}

bool http::Response::response_redirect(int socket, int status, std::string const &location)
{
	std::stringstream ss;
	setStatusLine(status);
	ss << _status_line << std::endl;
	ss << "Location: " << location << std::endl;
	ss << std::endl;

	send(socket, ss.str().c_str(), ss.tellp(), MSG_DONTWAIT);
	return (true);
}

void http::Response::set_default_err_page(std::string const &str)
{
	_default_err_page = str;
}


bool http::Response::response(int socket, int status, std::stringstream &file, 
	std::string const &content_type)
{
	std::stringstream ss;
	ss << file.str().length();
	setStatusLine(status);
	_header["Content-Type"] = content_type;
	_header["Content-Length"] = ss.str();
	_header["Connection"] = "Keep-Alive";
	_header["Keep-Alive"] = "timeout=5, max=20";

	ss.str("");
	ss << _status_line << std::endl;
	std::map<std::string, std::string>::const_iterator it = _header.begin();
	for(; it != _header.end(); it++) {
		ss << it->first << ": " << it->second << std::endl;
	}
	ss << std::endl;
	ss << file.str();
	ss << std::endl;
	send(socket, ss.str().c_str(), ss.tellp(), MSG_DONTWAIT);

	return (true);
}
