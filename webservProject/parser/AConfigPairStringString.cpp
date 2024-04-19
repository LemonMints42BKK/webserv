#include "Configs.hpp"

cfg::AConfigPairStringString::AConfigPairStringString(std::ifstream &file, std::string const &type) 
    : AConfig(type)
{
    (void) file;
}

cfg::AConfigPairStringString::~AConfigPairStringString() {}

std::string const & cfg::AConfigPairStringString::first() const
{
	return (_data.first);
}
std::string const & cfg::AConfigPairStringString::second() const
{
	return (_data.second);
}

std::pair<std::string, std::string> const & cfg::AConfigPairStringString::getData() const
{
	return (_data);
}

std::ostream & operator<<(std::ostream &o, cfg::AConfigPairStringString const &i)
{
	o << i.getType() << " " << i.first() << " " << i.second() << ";" << std::endl;
	return (o);
}
