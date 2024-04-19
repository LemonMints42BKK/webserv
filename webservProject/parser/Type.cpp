#include "Configs.hpp"

cfg::Type::Type(std::ifstream &file, std::string const &type) : AConfigVectorString(file, type)
{

}

cfg::Type::~Type() {}
