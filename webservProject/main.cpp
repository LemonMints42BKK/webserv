
#include "Configs.hpp"
#include "Server.hpp"
int main(void)
{
    // try{
    //     // std::ifstream file("default.conf");
    //     cfg::Configs configs("./parser/default.conf");
    //     configs.setGroupLevel(0, configs.begin(), configs.end());
    //     std::cout << configs << std::endl;


    //     cfg::Listens listens;
    //     configs.getListen(listens, configs.begin(), configs.end());
    //     for (cfg::Listens::const_iterator it = listens.begin();
    //         it != listens.end(); it++) {
    //             for (std::vector<std::string>::const_iterator itt = it->second.begin();
    //                 itt != it->second.end(); itt++) {
    //                     std::cout << "listen" << it->first << ":" << *itt << std::endl;
    //             }
    //     }

    // }
    // catch (std::exception const &e) {
    //     std::cerr << e.what() << std::endl;
    // }


    

    try {

        cfg::Configs configs("configs/default.conf");

        Server server(&configs);

        server.start_server();
    }
    catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}