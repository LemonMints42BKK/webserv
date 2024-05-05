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
    //   printf("Child process (pid: %d) hasn't exited yet...\n", child_pid);
      start_time = getTime();      
      if (post_time - start_time == 0)
        kill(child_pid, SIGKILL);
      sleep(1); // Check again after 1 second
    }
    return exited_pid;
}

void http::HttpV1::getExiteAndStatusForResponseByPath(pid_t exited_pid, int status, std::string path, std::stringstream &ss)
{
	if(exited_pid < 0) {
		std::cout << "CGI: failed" << std::endl;
		_stage = RESPONSED;
		_response->response(_socket, 500);
	}

	if (!WIFEXITED(status)) {
		std::cout << "CGI: failed" << std::endl;
		_stage = RESPONSED;
		_response->response(_socket, 500);
	} 
	int exit_status = WEXITSTATUS(status);
	if (exit_status != 0) {
		_stage = RESPONSED;
		std::cout << "CGI: failed" << std::endl;
		_response->response(_socket, 500);
	} 

	// std::cout << "CGI: success" << std::endl;
	_stage = RESPONSED;
	if(path == "/GenPage.py")
	{
		// std::cout << "CGI: success GenPage" << std::endl;
		_response->response(_socket, 200, ss, "text/html");
	}	
	if(path == "/upload.py")
		_response->response(_socket, 201);
	if(path == "/delete.py")
		_response->response(_socket, 200);
}

bool http::HttpV1::cgi()
{
	_stage = RESPONSED;
	pid_t pid = fork();
	if(pid == 0)
	{
		int pipefd[2];
		if (pipe(pipefd) == -1) {
			perror("pipe");
			_exit(1);
		}
		pid_t child_pid = fork();
		// FILE *file = tmpfile();
		// long filetmp = fileno(file);
		if (child_pid == 0) {
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);
			execveByPath("http/CgiFile/GenPage.py");
			_exit(0);
		} else {
			int status;
			close(pipefd[1]);
			pid_t exited_pid = wait_Child(child_pid, &status);
			std::stringstream buff;
			char buffer[1024];
			while(true) {
				int byte_read = read(pipefd[0], &buffer, 1024);
				if (byte_read == 0) break;
				if (byte_read < 0) {
					_response->response(_socket, 502);
				}
				buffer[byte_read] = 0;
				buff << buffer;
			}
			close(pipefd[0]);
			getExiteAndStatusForResponseByPath(exited_pid, status, "/GenPage.py", buff);
		}
		_exit(0);
	}	
	return (true);
}

long http::HttpV1::copyDataToFdTmp()
{
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
	return (filetmp);
}

void	http::HttpV1::execveByPath(std::string path)
{
	char *envp[] = {NULL};
	char *argv[6];
	argv[0] = strdup("/usr/bin/python3");
	argv[1] = strdup(path.c_str());
	argv[2] = strdup(_request->getMethod().c_str());
	argv[3] = strdup(_request->getHeader("Content-Type").c_str());
	argv[4] = strdup(_configs->getRoot(_request->getHeader("Host"), _request->getLocation()).c_str());
	argv[5] = NULL;
	if(execve(argv[0], argv, envp) == -1)
		_exit(1);
}
bool http::HttpV1::cgiUpload()
{
	_stage = RESPONSED;
	pid_t pid = fork();
	if(pid == 0)
	{
		pid_t child_pid = fork();
		if (child_pid == 0) {
			dup2(copyDataToFdTmp(), STDIN_FILENO);
			execveByPath("http/CgiFile/upload.py");
			_exit(0);
		} else {
			int status;
			pid_t exited_pid = wait_Child(child_pid, &status);
			getExiteAndStatusForResponseByPath(exited_pid, status, "/upload.py", _body);
		}
		_exit(0);
	}	
	return (true);
}

bool http::HttpV1::cgiDelete()
{
	_stage = RESPONSED;
	pid_t pid = fork();
	if(pid == 0)
	{
		pid_t child_pid = fork();
		if (child_pid == 0) {
			dup2(copyDataToFdTmp(), STDIN_FILENO);
			execveByPath("http/CgiFile/delete.py");
			_exit(0);
		} else {
			int status;
			pid_t exited_pid = wait_Child(child_pid, &status);
			getExiteAndStatusForResponseByPath(exited_pid, status, "/delete.py", _body);
		}
		_exit(0);
	}	
	return (true);
}

