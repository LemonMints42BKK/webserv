#ifndef HTTP_HPP
#define HTTP_HPP

#include "Configs.hpp"
#include <string>
#include <sstream>

namespace http
{
	#define HTTP_BUFFER 65000

	class Http
	{
		protected:
			int _socket;
			cfg::Configs const *_configs;
			std::stringstream _data;
		public:
			Http(int socket, cfg::Configs *configs);
			virtual bool readSocket() = 0;
			virtual ~Http();
	};

	class Httptest : public Http
	{
		public:
			Httptest(int socket, cfg::Configs *configs);
			bool readSocket();
			~Httptest();
	};
}



#endif
