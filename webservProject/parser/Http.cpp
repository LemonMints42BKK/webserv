#include "Configs.hpp"
#include <algorithm>

cfg::Http::Http(std::ifstream &file) : AGroup(file, "http")
{
	firstBracket(file);
	init(file);
	setServer();
	setTypes();
}

cfg::Http::~Http()
{

}

void cfg::Http::init(std::ifstream &file)
{
	while(true) {
		try {
			std::string directive;
			file.peek();
			if (file.eof()) 
				throw (std::runtime_error("Error: " + this->getType() + " '}' not found"));
			AConfig *dir;
			file >> directive;
			if (directive == "}") break;
			else if (directive == "server")
				dir = new Server(file);
			else if (directive == "types")
				dir = new Types(file);
			else
				throw(std::runtime_error(this->getType() + "Error: unknow directive " + directive));

			_configs.push_back(dir);
		} catch (std::exception const &e) {
			throw ;
		}
	}
}

void cfg::Http::setServer()
{
	Server *server;
	std::size_t n = 0;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if((server = dynamic_cast<cfg::Server*>(*it))) {
			if (_root.count((*server).getServerName()))
				throw (std::runtime_error ("validate http duplicate server"));
			n++;
			_root[server->getServerName()] = server->getLocation();
			_index[server->getServerName()] = server->getIndex();
			_allow[server->getServerName()] = server->getAllow();
			_client_max_body[server->getServerName()] = server->getClientMaxBody();
			_error_page[server->getServerName()] = server->getErrorPage();
			_listen[server->getServerName()] = server->getListen();
		}
	}
	if (!n)
		throw (std::runtime_error("validate http mush have server"));
}

std::string const & cfg::Http::getRoot(std::string const &server_name, std::string const &location)
{
	if (!_root.count(server_name)) 
		throw (std::runtime_error("getRoot server_name " + server_name + " not found"));
	if (!_root[server_name].count(location))
		return (_root[server_name][""]);
	return (_root[server_name][location]);
} 

std::vector<std::string> const & cfg::Http::getIndex(std::string const &server_name, 
	std::string const &location)
{
	if (!_index.count(server_name))
		throw (std::runtime_error("getIndex server_name " + server_name + " not found"));
	if (!_index[server_name].count(location))
		throw (std::runtime_error("getIndex location " + location + " not found"));
	
	//debug
	// for (std::vector<std::string>::const_iterator it = _index[server_name][location].begin();
	// 	it != _index[server_name][location].end(); it++) {
	// 		std::cout << "index: " << *it << std::endl;
	// }

	return (_index[server_name][location]);
}

std::vector<std::string> const & cfg::Http::getAllow(std::string const &server_name, 
	std::string const &location)
{
	if (!_allow.count(server_name))
		throw (std::runtime_error("getAllow server_name " + server_name + " not found"));
	if (!_allow[server_name].count(location))
		// throw (std::runtime_error("getAllow location " + location + " not found"));
		return (_allow[server_name][""]);
	return (_allow[server_name][location]);
}

int const & cfg::Http::getClientMaxBody(std::string const &server_name)
{
	if (!_client_max_body.count(server_name))
		throw (std::runtime_error("getClientMaxBody server_name " + server_name + " not found"));
	return (_client_max_body[server_name]);
}

std::string const & cfg::Http::getErrorPage(std::string const &server_name)
{
	if (!_error_page.count(server_name))
		throw (std::runtime_error("getClientMaxBody server_name " + server_name + " not found"));
	return (_error_page[server_name]);
}

std::vector<std::pair<std::string, std::string> >  
	const & cfg::Http::getListen(std::string server_name)
{
	if (!_listen.count(server_name))
		throw (std::runtime_error("getListen server_name " + server_name + " not found"));
	return (_listen[server_name]);
}

void cfg::Http::setTypes()
{
	Types *types;
	Type *type;
	for (config_itc it = _configs.begin(); it != _configs.end(); it++) {		
		if ((types = dynamic_cast<cfg::Types*>(*it))) {
			for (config_itc itt = types->begin(); itt != types->end(); itt++) {
				type = dynamic_cast<cfg::Type*>(*itt);
				_types[type->getType()].insert(_types[type->getType()].end(), type->begin(), type->end());
			}
		}
	}
}

std::string const cfg::Http::getTypes(std::string const &file) const
{
	std::string str = "application/octet-stream";
	std::size_t point = file.find_last_of('.');
	if (point == std::string::npos) return str;
	std::string extension = file.substr(point + 1);
	for (std::map<std::string, std::vector<std::string> >::const_iterator it = _types.begin();
		it != _types.end(); it++
	) {
		if (std::find(it->second.begin(), it->second.end(), extension) != it->second.end()) {
			str = it->first;
			break;
		}
	}
	return(str);
}

void cfg::Http::validate() const
{}
