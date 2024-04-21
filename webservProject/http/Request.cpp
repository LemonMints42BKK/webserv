#include "Http.hpp"

http::Request::Request()
{}

http::Request::~Request()
{}

void http::Request::setMethod(std::string const &method)
{
	_method = method;
}

std::string const& http::Request::getMethod() const
{
	return(_method);
}

void http::Request::setLocation(std::string const &location)
{
	_location = location;
}

std::string const& http::Request::getLocation() const
{
	return(_location);
}

bool http::Request::setHeader(std::string const &key, std::string const &value)
{
	if (_header.count(key)) return (false);
	_header[key] = value;
	return (true);
}


std::string const &http::Request::getHeader(std::string const &key)
{
	return (_header[key]);
}

std::map<std::string, std::string>::iterator http::Request::headerBegin()
{
	return (_header.begin());
}
std::map<std::string, std::string>::iterator http::Request::headerEnd()
{
	return (_header.end());
}

std::ostream & operator<<(std::ostream &o, http::Request &i)
{
	o << i.getMethod() << " " << i.getLocation() << std::endl;
	for (std::map<std::string, std::string>::iterator it = i.headerBegin();
		it != i.headerEnd(); it++) {
		o << it->first << ": " << it->second << std::endl;
	}
	return (o);
}