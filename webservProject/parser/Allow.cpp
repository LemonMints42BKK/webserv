#include "Configs.hpp"

cfg::Allow::Allow(std::ifstream &file) : AConfigVectorString(file, "allow")
{}

cfg::Allow::~Allow(){}