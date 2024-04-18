#include "Configs.hpp"

cfg::Index::Index(std::ifstream &file)
	: AConfigVectorString(file, "index")
{
}

cfg::Index::~Index() {}
