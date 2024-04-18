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
			~Httptest();
			bool readSocket();
	};

	class HttpV1 : public Http
	{
		private:
			int _stage;
			Request _request;
			bool endMessage(const char *buffer) const;
			bool parser();
			bool parserFirstLine();
			bool parserHeader();
			bool tryFiles();
			bool errorPage(int status);

		public:
			HttpV1(int socket, cfg::Configs *configs);
			~HttpV1();
			bool readSocket();
			std::string trim(std::string &string) const;
	};


}

#endif
