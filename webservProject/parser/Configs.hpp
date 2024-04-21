#ifndef CONFIGS_HPP
#define CONFIGS_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iostream>



namespace cfg
{
	typedef std::map<std::string, std::vector<std::string> > Listens;
	typedef std::vector<std::pair<std::string, std::string> > ListenPairs;

	class AConfig
	{
		private:
			std::string _type;
			AConfig();
			int _level;

		public:
			AConfig(std::string const &type);
			AConfig(AConfig const &ins);
			AConfig & operator=(AConfig const &rhs);
			virtual ~AConfig();
			std::string const & getType() const;
			void end_directive(std::ifstream &file);
			bool getString(std::string &buffer, std::ifstream &file);

			void setLevel(int level);
			int getLevel() const;
			std::string indent() const;
	};

	typedef std::vector<AConfig*>::const_iterator config_itc;

	/* ----------------------- group config ---------------------------*/

	class AConfigs : public AConfig
	{
		protected:
			std::vector<AConfig*> _configs;

			/* validate */
			void virtual validate() const = 0;
			std::size_t count_root(config_itc begin, config_itc end) const;

		public:
			AConfigs(std::string const &type);
			virtual ~AConfigs();
			config_itc begin() const;
			config_itc end() const;
			std::size_t size() const;

			void setGroupLevel(int n, config_itc begin, config_itc end);
			void getListen_getIp(Listens &listens, config_itc begin, config_itc end) const;
			// void getListen(Listens &listens, config_itc begin, config_itc end) const;
			// void getListenPairs(ListenPairs &listens, config_itc begin, config_itc end) const;
	};

	class AGroup : public AConfigs
	{
		protected:
			void virtual init(std::ifstream &file) = 0;
			void firstBracket(std::ifstream &file);
		public:
			AGroup(std::ifstream &file, std::string const &type);
			virtual ~AGroup();	
	};

	class Http;

	class Configs : public AConfigs
	{
		private:
			Http *_http;
			void validate() const;
			void setHttp();
		public:
			Configs(std::string const &filename);
			~Configs();
			std::string const & getRoot(std::string const &server_name, 
				std::string const &location) const;
			std::vector<std::string> const & getIndex(std::string const &server_name, 
				std::string const &location) const;
			std::vector<std::string> const & getAllow(std::string const &server_name, 
				std::string const &location) const;
			int const & getClientMaxBody(std::string const &server_name);
			std::string const & getErrorPage(std::string const &server_name);
			std::vector<std::pair<std::string, std::string> >  const & getListen(std::string server_name);
			std::string const getTypes(std::string const &file) const;
	};

	class Http : public AGroup
	{
		private:
			std::map<std::string, std::map<std::string, std::string> > _root;
			std::map<std::string, std::map<std::string, std::vector<std::string> > > _index;
			std::map<std::string, std::map<std::string, std::vector<std::string> > > _allow;
			std::map<std::string, int> _client_max_body;
			std::map<std::string, std::string > _error_page;
			std::map<std::string, std::vector<std::pair<std::string, std::string> > > _listen;
			std::map<std::string, std::vector<std::string> > _types;
			void init(std::ifstream &file);
			void validate() const;
			void setServer();
			void setTypes();
		public:
			Http(std::ifstream &file);
			~Http();
			std::string const & getRoot(std::string const &server_name, 
				std::string const &location);
			std::vector<std::string> const & getIndex(std::string const &server_name, 
				std::string const &location);
			std::vector<std::string> const & getAllow(std::string const &server_name, 
				std::string const &location);
			int const & getClientMaxBody(std::string const &server_name);
			std::string const & getErrorPage(std::string const &server_name);
			std::vector<std::pair<std::string, std::string> >  const & getListen(std::string server_name);
			std::string const getTypes(std::string const &file) const;
	};

	class Server : public AGroup
	{
		private:
			std::string _server_name;
			std::string _root;
			std::string _error_page;
			int _client_max_body;
			std::vector<std::pair<std::string, std::string> > _listen;
			std::map<std::string, std::string> _location; // <location, root>
			std::map<std::string, std::vector<std::string> > _index; // <location, indexs>
			std::map<std::string, std::vector<std::string> > _allow; // <location, indexs>
			void init(std::ifstream &file);
			void validate() const;
			void setServerName();
			void setListen();
			void setRoot();
			void setLocation();
			void setErrorPage();
			void setClientMaxBody();
			void setIndex();
			void setAllow();
		public:
			Server(std::ifstream &file);
			~Server();

			std::string const & getServerName() const;
			std::string const & getRoot() const;
			std::string const & getErrorPage() const;
			int const & getClientMaxBody() const;
			std::map<std::string, std::string> const & getLocation() const;
			std::map<std::string, std::vector<std::string> > const & getIndex() const;
			std::map<std::string, std::vector<std::string> > const & getAllow() const;
			std::vector<std::pair<std::string, std::string> > const & getListen() const;
			
	};

	class Location : public AGroup
	{
		private:
			std::string _location;
			std::string _root;
			std::vector<std::string> _index;
			std::vector<std::string> _allow;
			void init(std::ifstream &file);
			void validate() const;
			void setRoot();
			void setIndex();
			void setAllow();
		public:
			Location(std::ifstream &file);
			~Location();
			std::string const &getLocation() const;
			std::string const &getRoot() const;
			std::vector<std::string> const &getIndex() const;
			std::vector<std::string> const &getAllow() const;
	};

	class Types : public AGroup
	{
		private:
			void init(std::ifstream &file);
			void validate() const;
		public:
			Types(std::ifstream &file);
			~Types();
	};
	
	/* -------------------------- single config ---------------------------- */

	/* ---------------------------- Int ------------------------------------ */
	
	class AConfigInt : public AConfig
	{
		private:
			int _value;
		public:
			AConfigInt(std::ifstream &file, std::string const &type);
			virtual ~AConfigInt();
			int getValue() const;
	};

	class Worker_processes : public AConfigInt
	{
		public:
			Worker_processes(std::ifstream &file);
			~Worker_processes();
	};

	class Client_max_body : public AConfigInt
	{
		public:
			Client_max_body(std::ifstream &file);
			~Client_max_body();
	};
	

	/* ---------------------- String ------------------------------- */

	class AConfigString : public AConfig
	{
		private:
			std::string _str;
		public:
			AConfigString(std::ifstream &file, std::string const &type);
			virtual ~AConfigString();
			std::string const & str() const;
	};

	class Server_name : public AConfigString
	{
		public:
			Server_name(std::ifstream &file);
			~Server_name();
	};
	
	class Root : public AConfigString
	{
		public:
			Root(std::ifstream &file);
			~Root();
	};

	class Redirect : public AConfigString
	{
		public:
			Redirect(std::ifstream &file);
			~Redirect();
	};

	class Error_page : public AConfigString
	{
		public:
			Error_page(std::ifstream &file);
			~Error_page();
	};

	/* ---------------------- vecter<string> -------------------- */
	
	class AConfigVectorString : public AConfig
	{
		private:
			std::vector<std::string> _datas;
		public:
			AConfigVectorString(std::ifstream &file, std::string const &type);
			~AConfigVectorString();
			std::vector<std::string>::const_iterator begin() const;
			std::vector<std::string>::const_iterator end() const;
			std::vector<std::string> const & getDatas() const;
	};
	class Index : public AConfigVectorString
	{
		public:
			Index(std::ifstream &file);
			~Index();
	};

	class Allow : public AConfigVectorString
	{
		public:
			Allow(std::ifstream &file);
			~Allow();
	};

	class Type : public AConfigVectorString
	{
		public:
			Type(std::ifstream &file, std::string const &type);
			~Type();
	};


	/* ------------------------- pair string string ----------------------- */

	class AConfigPairStringString : public AConfig
	{
		protected:
			std::pair<std::string, std::string> _data;
			virtual void init(std::ifstream &file) = 0;
		public:
			AConfigPairStringString(std::ifstream &file, std::string const &type);
			~AConfigPairStringString();
			std::string const & first() const;
			std::string const & second() const;
			std::pair<std::string, std::string> const & getData() const;
	};

	class Listen : public AConfigPairStringString
	{
		private:
			void init(std::ifstream &file);
		public:
			Listen(std::ifstream &file);
			~Listen();
	};
}

std::ostream & operator<<(std::ostream &o, cfg::AConfigs const &i);
std::ostream & operator<<(std::ostream &o, cfg::AConfigInt const &i);
std::ostream & operator<<(std::ostream &o, cfg::AConfigVectorString const &i);
std::ostream & operator<<(std::ostream &o, cfg::AConfigPairStringString const &i);
std::ostream & operator<<(std::ostream &o, cfg::AConfigString const &i);

#endif