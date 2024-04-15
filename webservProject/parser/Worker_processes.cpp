#include "Configs.hpp"

cfg::Worker_processes::Worker_processes(std::ifstream &file)
	: AConfigInt(file, "worker_processes")
{
	(void) file;
}

cfg::Worker_processes::~Worker_processes()
{
}
