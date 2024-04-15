#include "Configs.hpp"

cfg::AConfigString::AConfigString(std::ifstream &file, std::string const &type)
	: AConfig(type)
{
	if (file.peek() <= 0) 
		throw (std::runtime_error("Error: " + this->getType()));
	if (file.eof()) 
		throw (std::runtime_error("Error: " + this->getType()));
	bool found_semicolon = getString(_str, file);
	if (!found_semicolon) end_directive(file);
}

cfg::AConfigString::~AConfigString() {}

std::string const & cfg::AConfigString::str() const
{
	return (_str);
}

std::ostream & operator<<(std::ostream &o, cfg::AConfigString const &i)
{
	o << i.getType() << " " << i.str() << ";" << std::endl;
	return (o);
}