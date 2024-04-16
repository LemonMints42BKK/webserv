#ifndef HTTP_HPP
#define HTTP_HPP

#include "Configs.hpp"
#include <string>
#include <sstream>

namespace http
{
	#define HTTP_BUFFER 65000
	#define HEADER 1
	#define BODY 2

	class Request;
	
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
			// int _stage;
			bool endMessage(const char *buffer) const;
			void parserHeader();

		public:
			HttpV1(int socket, cfg::Configs *configs);
			~HttpV1();
			bool readSocket();
			
	};

	class Request
	{
		private:
			std::string _method;
			std::string _location;
		public:
			Request();
			~Request();
	};
}

#endif
