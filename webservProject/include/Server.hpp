#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>

typedef struct Location
{
	std::string root;
	std::string index;
	std::string try_files;
	std::string rewrite;
	std::string error_page;
} Location;

typedef struct Config
{
	std::map<unsigned int, std::vector<unsigned short> > listen;
	std::string root;
	std::vector<std::string> index;
	std::map<std::string, Location> location;
} Config;

typedef std::map<unsigned int, std::vector<unsigned short> >::const_iterator Listen;
typedef std::vector<unsigned short>::const_iterator Port;


// this version is test on osi layer 4
class Server
{
private:
	Config _config;

	// Private method
	void __start_http(int socket);

	// void __parser_config(std::string const &Config_file);

	void __init_config(); // utils.cpp

public:

	// Constructor
	Server(); // Server.cpp
	Server(std::string const &config_file); // Server.cpp
	Server(Server const &ins); // Server.cpp
	~Server(); // Server.cpp

	// Overload operator
	Server & operator=(Server const &rhs); // Server.cpp
	
	// Member method
	void start_server(); // start_server.cpp
};

#endif