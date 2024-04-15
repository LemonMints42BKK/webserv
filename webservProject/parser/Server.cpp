#include "Configs.hpp"

cfg::Server::Server(std::ifstream &file) : AGroup(file, "server")
{
	firstBracket(file);
	init(file);
	setServerName();
	setListen();
	setRoot();
	setLocation();
	setErrorPage();
	setClientMaxBody();
	setIndex();
	// validate();
}

cfg::Server::~Server() {}

void cfg::Server::init(std::ifstream &file)
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
			else if (directive == "index")
				dir = new Index(file);
			else if (directive == "listen")
				dir = new Listen(file);
			else if (directive == "server_name")
				dir = new Server_name(file);
			else if (directive == "root")
				dir = new Root(file);
			else if (directive == "location")
				dir = new Location(file);
			else if (directive == "error_page")
				dir = new Error_page(file);
			else if (directive == "client_max_body")
				dir = new Client_max_body(file);
			else
				throw(std::runtime_error(this->getType() + "Error: unknow directive " + directive));

			_configs.push_back(dir);
		} catch (std::exception const &e) {
			throw ;
		}
	}
}

std::string const & cfg::Server::getServerName()const
{
	return (_server_name);
}

void cfg::Server::setServerName()
{
	/* server mush have 1 server_name */
	_server_name = "";
	std::size_t n = 0;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		Server_name *name;
		if ((name=dynamic_cast<Server_name*>(*it))) {
			n++ ;
			_server_name = (*name).str();
		}
		if (n > 1) throw(std::runtime_error("validate server_name on server"));
	}
	if (_server_name.length() == 0)
		throw(std::runtime_error("validate server_name on server"));
}

void cfg::Server::setListen()
{
	std::size_t n = 0;
	Listen *listen;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if ((listen=dynamic_cast<Listen*>(*it))) {
			n++ ;
			_listen.push_back(listen->getListen());
		}
	}
	if (!n) throw(std::runtime_error("validate listen on server"));
}

std::vector<std::pair<std::string, std::string> > const & cfg::Server::getListen() const
{
	return (_listen);
}

void cfg::Server::setRoot()
{
	std::size_t n = 0;
	_root = "";
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		Root *root;
		if ((root = dynamic_cast<cfg::Root*>(*it))) {
			_root = (*root).str();
			n++ ;
		}
		if (n > 1) throw (std::runtime_error("validate root on location"));
	}

	if (n == 0) {
		for (config_itc it = _configs.begin(); it != _configs.end(); it++) {
			Location *location;
			if ((location = dynamic_cast<cfg::Location*>(*it))) {
				if(!count_root((*location).begin(), (*location).end()))
					throw (std::runtime_error("validate root on location"));
			}
		}
	}
}

std::string const & cfg::Server::getRoot() const
{
	return (_root);
}

void cfg::Server::setLocation()
{
	std::size_t n = 0;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		Location *location;
		if((location = dynamic_cast<cfg::Location*>(*it))) {

			if(_location.count((*location).getLocation()))
				throw (std::runtime_error ("validate server duplicate location"));

			if((*location).getRoot().length())
				_location[(*location).getLocation()] = (*location).getRoot();
			else
				_location[(*location).getLocation()] = _root;
			n++ ;
		}
	}
	if(!n) throw (std::runtime_error ("validate server not have location"));
}

std::map<std::string, std::string> const & cfg::Server::getLocation() const
{
	return (_location);
}

void cfg::Server::setErrorPage()
{
	_error_page = "";
	Error_page *error_page;
	std::size_t n = 0;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if((error_page = dynamic_cast<Error_page*>(*it))) {
			_error_page = (*error_page).str();
			n++ ;
		}
	}
	if (n != 1) throw(std::runtime_error("validate server error_page"));
}

std::string const & cfg::Server::getErrorPage() const
{
	return (_error_page);
}

void cfg::Server::setClientMaxBody()
{
	_client_max_body = -1;
	Client_max_body *client_max_body;
	std::size_t n = 0;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if((client_max_body = dynamic_cast<Client_max_body*>(*it))) {
			_client_max_body = (*client_max_body).getValue();
			n++ ;
		}
	}
	if (n > 1) throw(std::runtime_error("validate server error_page"));
}

int const & cfg::Server::getClientMaxBody() const
{
	return (_client_max_body);
}

void cfg::Server::setIndex()
{
	Index *index;
	Location *location;
	std::vector<std::string> server_index;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if((index = dynamic_cast<cfg::Index*>(*it))) {
			server_index.insert(server_index.end(), index->begin(), index->end());
		}
	}
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if((location = dynamic_cast<cfg::Location*>(*it))) {
			if(location->getIndex().size())
				_index[location->getLocation()] = location->getIndex();
			else
				_index[location->getLocation()] = server_index;
		}
	}
}

std::map<std::string, std::vector<std::string> > const & cfg::Server::getIndex() const
{
	return (_index);
}

void cfg::Server::validate() const
{
}