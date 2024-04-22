
#include "Configs.hpp"
#include "Server.hpp"
#include <signal.h>


server::Server *server_program ;

void sig_handler(int signo)
{
    (void)signo;
    server_program->stop_server();
    delete server_program;
    _exit(0);
}

int main(int argc, char **argv)
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

    if (argc != 2)
        return (1);

    try {
        signal(SIGINT, sig_handler);
        cfg::Configs configs(argv[1]);

        // cfg::Configs configs("network/default.conf");

        //server::Server server(&configs);

        //server.start_server();
        server_program = new server::Server(&configs);
        server_program->start_server();
    }
    catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
