#include "Configs.hpp"

cfg::AConfigs::AConfigs(std::string const &type) : AConfig(type)
{
}

cfg::AConfigs::~AConfigs()
{
	cfg::config_itc it = this->begin();
	while (it != this->end()) {
		delete *it;
		it++;
	}
}

cfg::config_itc cfg::AConfigs::begin() const
{
	return _configs.begin();
}

cfg::config_itc cfg::AConfigs::end() const
{
	return _configs.end();
}

std::size_t cfg::AConfigs::size() const
{
	return (_configs.size());
}

void cfg::AConfigs::setGroupLevel(int n, config_itc begin, config_itc end)
{
	AConfigs *configs;
	while (begin != end) {

		(*begin)->setLevel(n);
		if ((configs = dynamic_cast<AConfigs*>(*begin))){
			setGroupLevel(n + 1, (*configs).begin(), (*configs).end());
		}
		begin++;
	}
}

void cfg::AConfigs::getListen_getIp(Listens &listens, config_itc begin, config_itc end) const
{
	AConfigs *configs;
	while (begin != end) {
		if ((*begin)->getType() == "listen") {
			cfg::Listen *listen = dynamic_cast<cfg::Listen*>(*begin); 
			listens[(*listen).first()].push_back((*listen).second());
		}
		if ((configs = dynamic_cast<AConfigs*>(*begin))) {
			getListen_getIp(listens, configs->begin(), configs->end());
		}
		begin++;
	}
}

// void cfg::AConfigs::getListenPairs(ListenPairs &listens, config_itc begin, config_itc end) const
// {
// 	AConfigs *configs;
// 	while (begin != end) {
// 		if ((*begin)->getType() == "listen") {
// 			cfg::Listen *listen = dynamic_cast<cfg::Listen*>(*begin);
// 			std::pair<std::string, std::string> p;
// 			p.first = (*listen).first();
// 			p.second = (*listen).second();
// 			listens.push_back(p);
// 		}
// 		if ((configs = dynamic_cast<AConfigs*>(*begin))) {
// 			getListenPairs(listens, configs->begin(), configs->end());
// 		}
// 		begin++;
// 	}
// }

std::size_t cfg::AConfigs::count_root(config_itc begin, config_itc end) const
{
	Root *root;
	std::size_t n = 0;
	for(;begin != end; begin++) {
		if ((root = dynamic_cast<cfg::Root*>(*begin)))
			n++;
	}
	return (n);
}

std::ostream & operator<<(std::ostream &o, cfg::AConfigs const &i)
{
	cfg::config_itc it = i.begin();
	cfg::config_itc it_in;
	cfg::AGroup *group;
	cfg::AConfigPairStringString *aConfigPairStringString;
	cfg::AConfigString *aConfigString;
	cfg::Location *location;
	cfg::AConfigVectorString *aConfigVectorString;
	
	while(it != i.end()) {

		o << (*it)->indent();
		
		if ((group = dynamic_cast<cfg::AGroup*>(*it))) {
			o << group->getType();
			if ((location = dynamic_cast<cfg::Location*>(*it))) {
				o << " " << location->getLocation();
			}
			o << " {" << std::endl;
			o << *group;
			o << group->indent() << "}" << std::endl;
		}

		if ((aConfigVectorString = dynamic_cast<cfg::AConfigVectorString*>(*it)))
			o << *aConfigVectorString << std::endl;

		if ((aConfigPairStringString = dynamic_cast<cfg::AConfigPairStringString*>(*it)))
			o << *aConfigPairStringString;

		if ((aConfigString = dynamic_cast<cfg::AConfigString*>(*it)))
			o << *aConfigString;

		it++;
	}
	return (o);
}
