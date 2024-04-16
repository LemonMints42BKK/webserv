#include "../include/Server.hpp"
#include <iostream>

// for test Server class
int main(void)
{
    try {
        // cfg::Configs configs("default.conf");
        Server server("default.conf");
        server.start_server();
    }
    catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
    
    return (0);
}

