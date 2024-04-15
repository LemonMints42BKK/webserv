#include "Configs.hpp"

cfg::Client_max_body::Client_max_body(std::ifstream &file)
	: AConfigInt(file, "client_max_body")
{
	(void) file;
}

cfg::Client_max_body::~Client_max_body()
{
}