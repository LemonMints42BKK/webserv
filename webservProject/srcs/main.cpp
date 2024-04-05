#include "Server.hpp"
#include <iostream>

// for test Server class
int main(void)
{
    Server server;

    try {
        server.start_server();
    }
    catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
