#include "Configs.hpp"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

void test_index(std::string const &server_name, std::string const &location, cfg::Configs &configs)
{
    std::vector<std::string> indexs = configs.getIndex(server_name, location);
    std::cout << "Index:";
    for (std::vector<std::string>::const_iterator it = indexs.begin(); it != indexs.end(); it++) {
            std::cout << " " << *it ;
    }
    std::cout << std::endl;
}

void test_root(std::string const &server_name, std::string const &location, cfg::Configs &configs)
{
    std::cout << "Root: " << configs.getRoot(server_name, location) << std::endl;
}

void test_server(std::string const &server_name, std::string const &location, cfg::Configs &configs)
{
    try{
        test_root(server_name, location, configs);
        test_index(server_name, location, configs);
    }
    catch (std::exception const &e) {
        std::cout << RED << e.what() << RESET << std::endl;
    }
}

void test_getLocation(std::string const &server_name, std::string const &location, cfg::Configs &configs) {
    cfg::Location *loc = configs.getLocation(server_name, location);
    if (loc)
        std::cout << "location: " << loc->getLocation() << std::endl;
    else
        std::cout << "location not found" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Use: ./prog configs_file" << std::endl;
    }
    try{
        // std::ifstream file("default.conf");
        cfg::Configs configs(argv[1]);
        configs.setGroupLevel(0, configs.begin(), configs.end());
        
        std::cout << configs << std::endl;

        // test_server("localhost:8080", "/", configs);
        // test_server("pnamnil", "/home", configs);
        // test_server("localhost", "/something", configs);

        test_getLocation("server", "location", configs);
        test_getLocation("localhost:8080", "/cgi", configs);
        test_getLocation("localhost", "/cgi", configs);

        // std::string index = configs.


        // cfg::Listens listens;
        // configs.getListen(listens, configs.begin(), configs.end());
        // for (cfg::Listens::const_iterator it = listens.begin();
        //     it != listens.end(); it++) {
        //         for (std::vector<std::string>::const_iterator itt = it->second.begin();
        //             itt != it->second.end(); itt++) {
        //                 std::cout << "listen" << it->first << ":" << *itt << std::endl;
        //         }
        // }

        // cfg::ListenPairs listens;
        // configs.getListenPairs(listens, configs.begin(), configs.end());
        // for(cfg::ListenPairs::const_iterator it = listens.begin();
        //     it != listens.end(); it++) {
        //         std::cout << "listen " << it->first << ":" << it->second << std::endl;
        // }

    }
    catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}