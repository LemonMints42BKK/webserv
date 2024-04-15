#include "Configs.hpp"

cfg::Http::Http(std::ifstream &file) : AGroup(file, "http")
{
	firstBracket(file);
	init(file);
	setServer();
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
		throw (std::runtime_error("getRoot location " + location + " not found"));
	return (_root[server_name][location]);
} 

std::vector<std::string> const & cfg::Http::getIndex(std::string const &server_name, 
	std::string const &location)
{
	if (!_index.count(server_name))
		throw (std::runtime_error("getIndex server_name " + server_name + " not found"));
	if (!_index[server_name].count(location))
		throw (std::runtime_error("getIndex location " + location + " not found"));
	return (_index[server_name][location]);
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

void cfg::Http::validate() const
{}
