#include "Http.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <iterator>
#include <fstream>
#include <fcntl.h>
time_t http::HttpV1::getTime()
{
	time_t timer;
    return time(&timer);
}

pid_t http::HttpV1::wait_Child(pid_t child_pid, int *status)
{
    pid_t exited_pid;
    time_t start_time = getTime();
    time_t post_time = start_time + 3;
    while ((exited_pid = waitpid(child_pid, status, WNOHANG)) == 0) {
      printf("Child process (pid: %d) hasn't exited yet...\n", child_pid);
      start_time = getTime();      
      if (post_time - start_time == 0)
        kill(child_pid, SIGKILL);
      sleep(1); // Check again after 1 second
    }
    return exited_pid;
}

void http::HttpV1::getExiteAndStatusForResponse(pid_t exited_pid, int status)
{
	if(exited_pid < 0) {
		std::cout << "CGI: failed" << std::endl;
		_stage = RESPONSED;
		_response->response(_socket, 500);
	}

	if (WIFEXITED(status)) {
		int exit_status = WEXITSTATUS(status);
		if (exit_status == 0) {
			std::cout << "CGI: success" << std::endl;
			_stage = RESPONSED;
			_response->response(_socket, 200, "./www/cgi.html", "text/html");
		} else {
			_stage = RESPONSED;
			std::cout << "CGI: failed" << std::endl;
			_response->response(_socket, 500);
		}
	} else {
		std::cout << "CGI: failed" << std::endl;
		_stage = RESPONSED;
		_response->response(_socket, 500);
	}
}

bool http::HttpV1::cgi()
{
	_stage = RESPONSED;
	pid_t pid = fork();
	if(pid == 0)
	{
		pid_t child_pid = fork();
		if (child_pid == 0) {
			char *envp[] = {NULL};
			// char *argv[3] = {"/usr/bin/python3", "./CgiFile/GenPage.py", NULL};
			char *argv[3];
			argv[0] = strdup("/usr/bin/python3");
			argv[1] = strdup("http/CgiFile/GenPage.py");
			argv[2] = NULL;
			if(execve(argv[0], argv, envp) == -1)
				_exit(1);
			_exit(0);
		} else {
			// Parent process
			int status;
			pid_t exited_pid = wait_Child(child_pid, &status);
			getExiteAndStatusForResponse(exited_pid, status);
		}
		_exit(0);
	}	
	return (true);
	// return (_response->response(_socket, 200, "./www/cgi_inconstruction.html", "text/html"));
}


// void copyStreamToFileDescriptor(std::stringstream& ss, int fd) {
//   std::streamsize n;
//   char buffer[BUFSIZ]; // Use a buffer size like BUFSIZ from stdio.h

//   n = ss.readsome(buffer, sizeof(buffer));
//   std::cout << "n: " << n << std::endl;
// }

bool http::HttpV1::cgiUpload()
{
	_stage = RESPONSED;
	pid_t pid = fork();
	if(pid == 0)
	{
		pid_t child_pid = fork();
		if (child_pid == 0) {
			FILE *file = tmpfile();
			int i = 0;

			std::stringstream buff ;
			while(i < _body.tellp()) {
				char buffer;
				_body.get(buffer);
				fputc(buffer, file);
				i++;
			}
			printf("i : %d\n", i);
			std::rewind(file);
			long filetmp = fileno(file);

			// char buffer[1];
			// i = 0;
			// int bytesread = 1;
			// while( bytesread > 0) {
			// 	bytesread = read(filetmp, buffer , 1);
			// 	i += bytesread;
			// }
    		// printf("i: %d\n", i);

			dup2(filetmp, STDIN_FILENO);
			char *envp[] = {NULL};
			char *argv[6];
			argv[0] = strdup("/usr/bin/python3");
			argv[1] = strdup("http/CgiFile/upload.py");
			argv[2] = strdup(_request->getMethod().c_str());
			argv[3] = strdup(_request->getHeader("Content-Type").c_str());
			argv[4] = strdup(_configs->getRoot(_request->getHeader("Host"), _request->getLocation()).c_str());
			argv[5] = NULL;
			if(execve(argv[0], argv, envp) == -1)
				_exit(1);
			_exit(0);
		} else {
			// Parent process
			int status;
			pid_t exited_pid = wait_Child(child_pid, &status);
			if(exited_pid < 0) {
				std::cout << "CGI: failed" << std::endl;
				_stage = RESPONSED;
				_response->response(_socket, 500);
			}

			if (WIFEXITED(status)) {
				int exit_status = WEXITSTATUS(status);
				if (exit_status == 0) {
					std::cout << "CGI: success" << std::endl;
					_stage = RESPONSED;
					_response->response(_socket, 200);
				} 
				else if(exit_status == 1)
				{
					std::cout << "CGI: failed" << std::endl;
					_stage = RESPONSED;
					_response->response(_socket, 404);
				}
				else {
					_stage = RESPONSED;
					std::cout << "CGI: failed" << std::endl;
					_response->response(_socket, 500);
				}
			} else {
				std::cout << "CGI: failed" << std::endl;
				_stage = RESPONSED;
				_response->response(_socket, 500);
			}

			// getExiteAndStatusForResponse(exited_pid, status);
		}
		_exit(0);
	}	
	return (true);
	// return (_response->response(_socket, 200, "./www/cgi_inconstruction.html", "text/html"));
}

bool http::HttpV1::cgiDelete()
{
_stage = RESPONSED;
	pid_t pid = fork();
	if(pid == 0)
	{
		pid_t child_pid = fork();
		if (child_pid == 0) {
			FILE *file = tmpfile();
			int i = 0;

			std::stringstream buff ;
			while(i < _body.tellp()) {
				char buffer;
				_body.get(buffer);
				fputc(buffer, file);
				i++;
			}
			printf("i : %d\n", i);
			std::rewind(file);
			long filetmp = fileno(file);

			dup2(filetmp, STDIN_FILENO);
			char *envp[] = {NULL};
			char *argv[6];
			argv[0] = strdup("/usr/bin/python3");
			argv[1] = strdup("http/CgiFile/delete.py");
			argv[2] = strdup(_request->getMethod().c_str());
			argv[3] = strdup(_request->getHeader("Content-Type").c_str());
			argv[4] = strdup(_configs->getRoot(_request->getHeader("Host"), _request->getLocation()).c_str());
			argv[5] = NULL;
			if(execve(argv[0], argv, envp) == -1)
				_exit(1);
			_exit(0);
		} else {
			// Parent process
			int status;
			pid_t exited_pid = wait_Child(child_pid, &status);
			if(exited_pid < 0) {
				std::cout << "CGI: failed" << std::endl;
				_stage = RESPONSED;
				_response->response(_socket, 500);
			}


			// getExiteAndStatusForResponse(exited_pid, status);
		}
		_exit(0);
	}	
	return (true);
}
