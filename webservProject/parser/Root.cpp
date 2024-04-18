#include "Configs.hpp"

cfg::Root::Root(std::ifstream &file) : AConfigString(file, "root")
{
}

cfg::Root::~Root() {}
