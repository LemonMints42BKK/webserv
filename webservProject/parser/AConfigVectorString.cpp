#include "Configs.hpp"

cfg::AConfigVectorString::AConfigVectorString(std::ifstream &file, std::string const &type)
    : AConfig(type)
{
    while (true) 
    {
    	if (file.peek() <= 0) 
			throw (std::runtime_error("Error: " + this->getType()));
		if (file.eof()) 
			throw (std::runtime_error("Error: " + this->getType()));
		
		std::string buffer;

		bool found_semicolon = getString(buffer, file);
		if (buffer.length()) _datas.push_back(buffer);
		if (found_semicolon) break;
    }
}

cfg::AConfigVectorString::~AConfigVectorString() {}

std::vector<std::string>::const_iterator cfg::AConfigVectorString::begin() const
{
	return (_datas.begin());
}

std::vector<std::string>::const_iterator cfg::AConfigVectorString::end() const
{
	return (_datas.end());
}

std::vector<std::string> const & cfg::AConfigVectorString::getDatas() const
{
	return (_datas);
}

std::ostream & operator<<(std::ostream &o, cfg::AConfigVectorString const &i)
{
    o << i.getType();

	std::vector<std::string>::const_iterator it = i.begin();
    while (it != i.end()) {
		o << " " << *it;
		it ++;
	}
	o << ";";
	return (o);
}
