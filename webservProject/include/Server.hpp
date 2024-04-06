#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <sys/select.h>
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
	std::map<std::string , std::vector<std::string> > listen;
	std::string root;
	std::vector<std::string> index;
	std::map<std::string, Location> location;
} Config;

typedef std::map<std::string, std::vector<std::string> >::const_iterator Listen;
typedef std::vector<std::string>::const_iterator Port;


// this version is test on osi layer 4
class Server
{
private:
	Config _config;

	// Private method
	void __start_http(int socket);

	// void __parser_config(std::string const &Config_file);

	void __init_config(); // utils.cpp
	// start server group
	fd_set _working_set;
	fd_set _master_set;
	int _max_sd;
	bool _end_server;
	std::vector<int> socketlist;
	
	int	__createNewSocket(std::string ip ,std::string port); // startserver.cpp
	void __getInputAndCreateListSocket(void); // startserver.c
	void __setUpMoniterSocket(void); // startserver.cpp
	void __runMoniter(void);// startserver.cpp
	void __loopCheckFd_workingSet(void); //startserver.cpp
	// void __readDataFromClient(int i,bool &close_conn); // startserver.cpp
	void __requestFromClient(int socket); // startserver.cpp
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