#include "Configs.hpp"

cfg::Listen::Listen(std::ifstream &file) : AConfig("listen")
{
    if (file.peek() <= 0) 
	{
        throw (std::runtime_error("Error: " + this->getType()));
	}

	std::string buffer;

	bool found_semicolon = getString(buffer, file);

	std::size_t sep = buffer.find(':');
	if (sep != std::string::npos)
	{
		_listen.first = buffer.substr(0, sep);
		_listen.second = buffer.substr(sep + 1);
	}
	else
	{
		_listen.first = "0.0.0.0";
		_listen.second = buffer;
	}
	if (!found_semicolon) end_directive(file);
}

cfg::Listen::~Listen() {}

std::string const & cfg::Listen::first() const
{
	return (_listen.first);
}
std::string const & cfg::Listen::second() const
{
	return (_listen.second);
}

std::pair<std::string, std::string> const & cfg::Listen::getListen() const
{
	return (_listen);
}

std::ostream & operator<<(std::ostream &o, cfg::Listen const &i)
{
	o << i.getType() << " " << i.first() << ":" << i.second() << ";" << std::endl;
	return (o);
}
