#include "Configs.hpp"

cfg::Server_name::Server_name(std::ifstream &file) : AConfigString(file, "server_name")
{
    (void) file;
}

cfg::Server_name::~Server_name() {}