#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <sys/socket.h>
#include <stdio.h>
#include <iostream>
class Http
{   
    private:
        int _socket;
        std::string _request;
        std::string _response;
        int _total_bytes;
    public:
        Http(int socket){
            _socket = socket;
            _total_bytes = 0;
        };
        ~Http(){};
        void recordRequest(char &buffer, int len){
                _request.append(buffer, len);
        };
        void sendResponse(){
            // if(_socket == 6)
            std::cout << "sending response" << std::endl;
            std::cout << _socket << std::endl;
            send(_socket, "HTTP/1.1 200 OK\n", 16, 0);
            // if(_socket == 5)
            // {
            //     printf("  %lu bytes received\n", _request.length());
            //     printf(" total bytes received: %d\n", _total_bytes);
            //     // send(_socket, "HTTP/1.1 200 OK\n", 16, 0);
            
            // }
            printf(" total bytes received: %d\n", _total_bytes);
        };
        void incrementTotalBytes(int len){
            _total_bytes += len;
        };

        int getTotalBytes(){
            return _total_bytes;
        };
};
#endif