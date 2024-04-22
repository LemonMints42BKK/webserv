#include "Configs.hpp"

cfg::Cgi_exe::Cgi_exe(std::ifstream &file) : AConfigString(file, "cgi_exe")
{}

cfg::Cgi_exe::~Cgi_exe(){}