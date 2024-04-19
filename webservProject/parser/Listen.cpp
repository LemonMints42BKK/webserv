#include "Configs.hpp"

cfg::Listen::Listen(std::ifstream &file) : AConfigPairStringString(file, "listen")
{
    init(file);
}

cfg::Listen::~Listen() {}

void cfg::Listen::init(std::ifstream &file)
{
	if (file.peek() <= 0) 
	{
        throw (std::runtime_error("Error: " + this->getType()));
	}

	std::string buffer;

	bool found_semicolon = getString(buffer, file);

	std::size_t sep = buffer.find(':');
	if (sep != std::string::npos)
	{
		_data.first = buffer.substr(0, sep);
		_data.second = buffer.substr(sep + 1);
	}
	else
	{
		_data.first = "0.0.0.0";
		_data.second = buffer;
	}
	if (!found_semicolon) end_directive(file);
}
