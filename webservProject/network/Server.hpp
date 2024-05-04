#ifndef SERVER_HPP
#define SERVER_HPP

#include "Configs.hpp"
#include "Http.hpp"


#include <string>
#include <vector>
#include <map>
#include <sys/select.h>


typedef std::map<std::string, std::vector<std::string> >::const_iterator Listen;
typedef std::vector<std::string>::const_iterator Port;

struct InfoSocket
{
	int socket;
	std::string ip;
	std::string port;
};

// this version is test on osi layer 4
namespace server
{
	class Server
	{
	private:
		cfg::Configs *_configs;
		time_t _time[FD_SETSIZE];
		http::Http *_http[FD_SETSIZE];

		void __init_config(); // utils.cpp
		//manage in select
		fd_set _working_set;
		int log_output;
		fd_set _master_set;
		fd_set _master_set_write;
		int _max_sd;
		bool _end_server;
		bool _close_conn;
		std::vector<int> socketlist;
		//data socket
		// std::pair<std::string, std::string> server;
		// std::pair<std::string, std::string> client;


		int	__createNewSocket(std::string ip ,std::string port); // createNewSocket.cpp
		void __getNewSocketfd(int &sockfd); // createNewSocket.cpp
		void __closeAllSocketlist(void); // createNewSocket.cpp
		void __setReuseAddr(int &sockfd); // createNewSocket.cpp
		void __getInformAndBind(int &sockfd, std::string &ip, std::string &port); // createNewSocket.cpp
		void __checkClientTimeOut(void); // createNewSocket.cpp

		void __getInputAndCreateListSocket(void); // startserver.c
		void __setUpMoniterSocket(void); // startserver.cpp
		void __runMoniter(void);// startserver.cpp
		void __loopCheckFd_workingSet(void); //startserver.cpp
		void __setNonBlocking(int socket); // startserver.cpp
		time_t  __getTime(); // startserver.cpp
		bool __checkIsSocketListen(size_t &socket); // startserver.cpp


		// void __CloseSocketFdListenInMasterSet(void); // startserver.cpp
		void __setCleanupSocket(void); // startserver.cpp
		// bool __getCheckMaster_AllZero(void); // startserver.cpp
		void __handle_close_conn(size_t i); // startserver.cpp
		// void __readDataFromClient(int i,bool &close_conn); // startserver.cpp
		void __requestFromClient(int socket); // startserver.cpp
	public:

		Server(cfg::Configs *configs);
		Server(); // Server.cpp
		// Server(std::string const &config_file); // Server.cpp
		~Server(); // Server.cpp
		
		// Member method
		void start_server(); // start_server.cpp
		void stop_server(); // start_server.cpp
	};	
}
#endif