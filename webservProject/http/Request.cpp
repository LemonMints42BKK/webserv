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
	if (_header.count(key)) return (_header[key]);
	return ("");
}
