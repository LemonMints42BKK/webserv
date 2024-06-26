#include "Configs.hpp"

cfg::Location::Location(std::ifstream &file) : AGroup(file, "location")
{
	// parser location
	if (file.peek() <= 0) 
		throw (std::runtime_error("Error: " + this->getType()));
	if (file.eof()) 
		throw (std::runtime_error("Error: " + this->getType()));
	bool found_semicolon = getString(_location, file);
	if (found_semicolon)
		throw (std::runtime_error("Error: " + this->getType()));

	firstBracket(file);
	init(file);
	setRoot();
	setIndex();
	setAllow();
	setCgi();
	setRedirect();
	// validate();
}

cfg::Location::~Location(){}

void cfg::Location::init(std::ifstream &file)
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
			else if (directive == "root")
				dir = new Root(file);
			else if (directive == "redirect")
				dir = new Redirect(file);
			else if (directive == "allow")
				dir = new Allow(file);
			else if (directive == "cgi_file")
				dir = new Cgi_file(file);
			else if (directive == "cgi_exe")
				dir = new Cgi_exe(file);
			else
				throw(std::runtime_error(this->getType() + "Error: unknow directive " + directive));

			_configs.push_back(dir);
		} catch (std::exception const &e) {
			throw ;
		}
	}
}

std::string const & cfg::Location::getLocation() const
{
	return (_location);
}

void cfg::Location::setRoot()
{
	std::size_t n = 0;
	_root = "";
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		Root *root;
		if ((root = dynamic_cast<cfg::Root*>(*it))) {
			_root = root->str();
			n++ ;
		}
	}
	if (n > 1) throw (std::runtime_error("validate root on location"));
}

std::string const & cfg::Location::getRoot() const
{
	return (_root);
}

void cfg::Location::setIndex()
{
	Index *index;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if((index = dynamic_cast<Index*>(*it))) {
			_index.insert(_index.end(), index->begin(), index->end());
		}
	}
	// debug
	// for (std::vector<std::string>::const_iterator it = _index.begin();
	// it != _index.end(); it++) {
	// 	std::cout << "location setIndex index: " << *it << std::endl;
	// }
}

std::vector<std::string> const & cfg::Location::getIndex() const
{
	return (_index);
}

void cfg::Location::setAllow()
{
	Allow *allow;
	std::size_t n = 0;
	for(config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if((allow = dynamic_cast<cfg::Allow*>(*it))) {
			_allow = allow->getDatas();
			n++;
		}
		if (n > 1) throw (std::runtime_error("validate allow on location"));
	}
}

std::vector<std::string> const &cfg::Location::getAllow() const{
	return (_allow);
}

void cfg::Location::setCgi()
{
	Cgi_file *cgi_file;
	Cgi_exe *cgi_exe;
	std::size_t f = 0;
	std::size_t e = 0;
	_cgi = false;
	for (config_itc it = _configs.begin(); it != _configs.end(); it++) {
		if ((cgi_file = dynamic_cast<Cgi_file*>(*it))) {
			_cgi_file = cgi_file->str();
			f++;
			if (f > 1) throw (std::runtime_error("validate cgi_file on location"));
		}
		if ((cgi_exe = dynamic_cast<Cgi_exe*>(*it))) {
			_cgi_exe = cgi_exe->str();
			e++;
			if (e > 1) throw (std::runtime_error("validate cgi_file on location"));
		}
	}
	if (f != e) throw (std::runtime_error("validate cgi_file on location"));
	if (f) _cgi = true;
}

std::string const &cfg::Location::getCgiFile() const
{
	return (_cgi_file);
}

std::string const &cfg::Location::getCgiExe() const
{
	return (_cgi_exe);
}

bool cfg::Location::isCgi() const
{
	return (_cgi);
}

void cfg::Location::setRedirect()
{
	Redirect * re;
	for (config_itc it = _configs.begin(); it != _configs.end(); it++)
	{
		if ((re = dynamic_cast<Redirect*>(*it))) {
			_redirect = re->str();
		}
	}
}

std::string const &cfg::Location::getRedirect() const\
{
	return (_redirect);
}



void cfg::Location::validate() const
{
}
