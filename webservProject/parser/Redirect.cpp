#include "Configs.hpp"

cfg::Redirect::Redirect(std::ifstream &file) : AConfigString(file, "redirect")
{
    (void) file;
}

cfg::Redirect::~Redirect() {}
