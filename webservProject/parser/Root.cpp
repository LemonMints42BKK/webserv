#include "Configs.hpp"

cfg::Root::Root(std::ifstream &file) : AConfigString(file, "root")
{
    (void) file;
}

cfg::Root::~Root() {}
