#include "Configs.hpp"

cfg::Index::Index(std::ifstream &file) : AConfig("index")
{
    while (true) 
    {
    	if (file.peek() <= 0) 
			throw (std::runtime_error("Error: " + this->getType()));
		if (file.eof()) 
			throw (std::runtime_error("Error: " + this->getType()));
		
		std::string buffer;

		bool found_semicolon = getString(buffer, file);
		if (buffer.length()) _indexs.push_back(buffer);
		if (found_semicolon) break;
    }
}

cfg::Index::~Index() {}

std::vector<std::string>::const_iterator cfg::Index::begin() const
{
	return (_indexs.begin());
}

std::vector<std::string>::const_iterator cfg::Index::end() const
{
	return (_indexs.end());
}

std::ostream & operator<<(std::ostream &o, cfg::Index const &i)
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
