#ifndef HTTP_HPP
#define HTTP_HPP

#include "Configs.hpp"
#include <string>
#include <sstream>

namespace http
{
	#define HTTP_BUFFER 65000
	#define METHOD 1
	#define HEADER 2
	#define BODY 3

	class Request
	{
		private:
			std::string _method;
			std::string _location;
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
	};

	class Httptest : public Http
	{
		public:
			Httptest(int socket, cfg::Configs *configs);
			bool readSocket();
			void sendResponse();
			~Httptest();
	};

	class HttpV1 : public Http
	{
		private:
			int _stage;
			Request _request;
			Response _response;
			// bool endMessage(const char *buffer) const;
			bool parser();
			bool parserFirstLine();
			bool parserHeader();
			bool tryFiles();
			bool fileExists(const std::string& filename);
			bool isDirectory(const std::string& filename);
			bool isFile(const std::string& filename);
			// bool errorPage(int status);

		public:
			HttpV1(int socket, cfg::Configs *configs);
			~HttpV1();
			bool readSocket();
			std::string trim(std::string &string) const;
	};
}

std::ostream & operator<<(std::ostream &o, http::Request &i);

#endif
