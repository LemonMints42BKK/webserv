#ifndef SERVER_HPP
#define SERVER_HPP

#include "Configs.hpp"
#include "Http.hpp"


#include <string>
#include <vector>
#include <map>
#include <sys/select.h>


typedef struct Location
{
	std::string root;
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
	cfg::Configs *_configs;
	http::Http *_http[FD_SETSIZE];

	
	// Private method
	// void __start_http(int socket);

	// void __parser_config(std::string const &Config_file);

	void __init_config(); // utils.cpp
	// start server group
	fd_set _working_set;
	int log_output;
	fd_set _master_set;
	fd_set _master_set_write;
	int _max_sd;
	bool _end_server;
	bool _close_conn;
	std::vector<int> socketlist;
	
	int	__createNewSocket(std::string ip ,std::string port); // startserver.cpp
	void __getInputAndCreateListSocket(void); // startserver.c
	void __setUpMoniterSocket(void); // startserver.cpp
	void __runMoniter(void);// startserver.cpp
	void __loopCheckFd_workingSet(void); //startserver.cpp
	void __setNonBlocking(int socket); // startserver.cpp

	void __setCloseSocketFdListen(void); // startserver.cpp
	void __setOpenSocketFdListen(void); // startserver.cpp
	
	bool __getCheckMaster_AllZero(void); // startserver.cpp
	void __handle_close_conn(size_t i); // startserver.cpp
	// void __readDataFromClient(int i,bool &close_conn); // startserver.cpp
	void __requestFromClient(int socket); // startserver.cpp
public:

	Server(cfg::Configs *configs);
	// Constructor
	Server(); // Server.cpp
	// Server(std::string const &config_file); // Server.cpp
	Server(Server const &ins); // Server.cpp
	~Server(); // Server.cpp
	Server(const std::string name_file); // get config from parser server.cpp
	// Overload operator
	Server & operator=(Server const &rhs); // Server.cpp
	
	// Member method
	void start_server(); // start_server.cpp
};

#endif