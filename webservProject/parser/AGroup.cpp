#include "Configs.hpp"

cfg::AGroup::AGroup(std::ifstream &file, std::string const &type) 
    : AConfigs(type)
{
	(void) file;
}

cfg::AGroup::~AGroup(){}

void cfg::AGroup::firstBracket(std::ifstream &file)
{
	std::string directive;
	file.peek();

	if (file.eof()) 
		throw (std::runtime_error("Error: " + this->getType()));
	
	file >> directive;
	
	if (directive != "{") 
		throw (std::runtime_error("Error: " + this->getType() + " '{' not found"));
}
