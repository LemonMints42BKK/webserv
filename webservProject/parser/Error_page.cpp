#include "Configs.hpp"

cfg::Error_page::Error_page(std::ifstream &file) : AConfigString(file, "error_page")
{
    (void) file;
}

cfg::Error_page::~Error_page() {}
