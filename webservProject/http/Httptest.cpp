#include "Http.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>


#include <iostream>
#include <sstream>
#include <ctime>
http::Httptest::Httptest(int socket, cfg::Configs *configs) : Http(socket, configs)
{
    (void) _configs;
    (void) _socket;
};

http::Httptest::~Httptest()
{
    std::cout << "Httptest destructor called" << std::endl;
}


void http::Httptest::sendResponse()
{
    std::stringstream oss;
	std::string message = "Hello this message sent from PTP";
	oss << "HTTP/1.1 200 OK" << std::endl
		<< "Server: PTP" << std::endl
		<< "Content-Type: text/plan;" << std::endl
		<< "Content-Length: " << message.length() << std::endl
		<< std::endl
		<< message
		<< std::endl
		<< std::endl;
	send(_socket, oss.str().c_str(), oss.str().length(), 0);

}

bool http::Httptest::readSocket()
{
    std::cout << "Httptest readSocker called" << std::endl;
	int rc;
	char buffer[65000];
	// std::string request_line;
	// std::string temp;
//    __setBlocking(i);

// _http[socketfd].read_socket;
// while will replace http::read_Socket
    while (true)
    {
            // setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, NULL, 0);
            rc = recv(_socket, buffer, sizeof(buffer), 0);
            std::cout << "HTTP: " << rc << std::endl;
            if (rc < 0)
                return false;
            if (rc == 0)
            {
                printf("  Connection closed\n");
                return true;
            }
            _data.write(buffer, rc);
            std::cout << "Http length total:" << _data.tellp() << std::endl;
            sendResponse();
            // _http[_socket]->incrementTotalBytes(len);
            // _http[_socket]->sendResponse();
            // sendResponse();
    }
    return false;
}


double http::Httptest::getTime()
{
    time_t timer;
    struct tm y2k ;

    y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

    time(&timer);  /* get current time; same as: timer = time(NULL)  */

    return difftime(timer,mktime(&y2k));
}

void http::Httptest::cgi()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            // Child process code (e.g., sleep for some time)
            
            sleep(1);
            _exit(0);
        } 
        else 
        {
            // Parent process
            int status;
            pid_t exited_pid;
          
            double start = getTime();
            double post = start + 3;
            while ((exited_pid = waitpid(child_pid, &status, WNOHANG)) == 0) {
                // time(&timer);
                printf("Child process (pid: %d) hasn't exited yet...\n", child_pid);
                start = getTime();
                // printf ("%.f seconds since January 1, 2000 in the current timezone\n", post));
                printf("%.f",post-start);
                if (post - start == 0)
                {
                    kill(child_pid, SIGKILL);
                }
                sleep(1); // Check again after 1 second
            }

            if (exited_pid > 0) {
                if (WIFEXITED(status)) {
                    // Child process exited normally
                    _response.response(_socket, 200);
                } else if (WIFSIGNALED(status)) {
                    // Child process exited due to signal
                    _response.response(_socket, 500);
                }
            } 
            else {
                _response.response(_socket, 500);
            }
        }
    }
}