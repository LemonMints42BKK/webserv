#include "Configs.hpp"

cfg::Types::Types(std::ifstream &file) : AGroup(file, "types")
{
    firstBracket(file);
    init(file);
}

cfg::Types::~Types()
{}

void cfg::Types::init(std::ifstream &file)
{
	while(true) {
		try {
			file.peek();
			std::string directive;
			file >> directive;
			if (directive == "}") break;
			if (file.eof())
				throw (std::runtime_error("Error: " + this->getType() + " '}' not found"));
			AConfig *dir = new Type(file, directive);
			_configs.push_back(dir);
		}
		catch (std::exception const &e) {
			throw ;
		}
	}
}

void cfg::Types::validate() const {}
