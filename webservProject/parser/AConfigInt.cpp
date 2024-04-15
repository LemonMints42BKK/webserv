#include "Configs.hpp"

cfg::AConfigInt::AConfigInt(std::ifstream &file, std::string const & type)
	: AConfig(type)
{
	if (file.peek() <= 0) throw (std::runtime_error("Error: " + this->getType()));
	file >> _value;
	if (file.fail()) throw (std::runtime_error("Error: " + this->getType()));
	end_directive(file);
}

cfg::AConfigInt::~AConfigInt()
{

}

int cfg::AConfigInt::getValue() const
{
	return (_value);
}

std::ostream & operator<<(std::ostream &o, cfg::AConfigInt const &i)
{
	o << i.getType() << " " << i.getValue() << ";";
	return (o);
}