#ifndef HTTP_HPP
#define HTTP_HPP

#include "Configs.hpp"
#include <string>
#include <sstream>

namespace http
{
	#define HTTP_BUFFER 65000
	#define START_LINE 1
	#define HEADER 2
	#define BODY 3
	#define RESPONSED 4

	class Request
	{
		private:
			std::string _method;
			std::string _location;
			std::string _cgi_file;
			std::string _cgi_exe;
			std::map<std::string, std::string> _header;
		public:
			Request();
			~Request();

			void setMethod(std::string const &method);
			std::string const &getMethod() const;

			void setLocation(std::string const &location);
			std::string const &getLocation() const;

			bool setHeader(std::string const &key, std::string const &value);
			std::string const &getHeader(std::string const &key);

			void setCgiFile(std::string const &cgi_file);
			void setCgiExe(std::string const &cgi_Exe);
			std::string const &getCgiFile() const;
			std::string const &getCgiExe() const;

			std::map<std::string, std::string>::iterator headerBegin();
			std::map<std::string, std::string>::iterator headerEnd();
	};

	class Response
	{
		private:
			std::string _status_line;
			std::string _content;
			void setStatusLine(int status);
			std::map<int, std::string> _status;
			std::map<std::string, std::string> _header;
		public:
			Response();
			~Response();
			bool response(int socket, int status, std::string const &filename, 
				std::string const &content_type);
			bool response(int socket, int status);
			void setContent(std::string const &file);
			// void setHeader(std::string, std::string);

	};

	class ResponseV2
	{
		private:
			std::string _status_line;
			std::string _content;
			void setStatusLine(int status);
			std::map<int, std::string> _status;
			std::map<std::string, std::string> _header;
			std::stringstream _response; 
		public:
			ResponseV2();
			~ResponseV2();
			bool response(int socket, int status, std::string const &filename, 
				std::string const &content_type);
			bool response(int socket, int status);
			void setContent(std::string const &file);
			// void setHeader(std::string, std::string);
			bool writeSocket(int socket);
	};
	
	class Http
	{
		protected:
			int _socket;
			cfg::Configs const *_configs;
			std::stringstream _data;
		public:
			Http(int socket, cfg::Configs *configs);
			virtual ~Http();
			virtual bool readSocket() = 0;
			virtual bool writeSocket() = 0;
			int getSocket() const;
	};

	class Httptest : public Http
	{
		private:
			Response _response;
		public:
			Httptest(int socket, cfg::Configs *configs);
			bool readSocket();
			void sendResponse();
			~Httptest();

			void cgi();
			double getTime();
			void readSocket_withCgi();
	};

	class HttpV1 : public Http
	{
		private:
			int _stage;
			Request *_request;
			Response *_response;
			bool parser();
			bool parserFirstLine();
			bool parserHeader();
			bool router();
			bool cgi();
			bool tryFiles();
			bool fileExists(const std::string& filename);
			bool isDirectory(const std::string& filename);
			bool isFile(const std::string& filename);
			
			void getExiteAndStatusForResponse(pid_t exited_pid, int status);
			pid_t wait_Child(pid_t child_pid, int *status);
			time_t getTime();
			bool cgiUpload();
			// bool errorPage(int status);


		public:
			HttpV1(int socket, cfg::Configs *configs);
			~HttpV1();
			bool readSocket();
			bool writeSocket();
			std::string trim(std::string &string) const;
	};

	class HttpV2 : public Http
	{
		private:
			int _stage;
			Request *_request;
			ResponseV2 *_response;
			bool parser();
			bool parserFirstLine();
			bool parserHeader();
			bool router();
			bool cgi();
			bool tryFiles();
			bool fileExists(const std::string& filename);
			bool isDirectory(const std::string& filename);
			bool isFile(const std::string& filename);
			// bool errorPage(int status);


		public:
			HttpV2(int socket, cfg::Configs *configs);
			~HttpV2();
			bool readSocket();
			bool writeSocket();
			std::string trim(std::string &string) const;
	};
}

std::ostream & operator<<(std::ostream &o, http::Request &i);

#endif
