#include "Configs.hpp"

cfg::Cgi_file::Cgi_file(std::ifstream &file) : AConfigString(file, "cgi_file")
{}

cfg::Cgi_file::~Cgi_file(){}
