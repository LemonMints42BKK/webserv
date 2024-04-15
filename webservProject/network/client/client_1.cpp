#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
void sentDataHuge(int sockfd)
{
	int sum = 0;
   std::ifstream file("/home/spipitku/goinfre/myfile");
   const char *data = "Hello, world!";
   int flags = MSG_DONTWAIT;

   std::ostringstream oss;
   oss << file.rdbuf();
   // int sum = 0;
   while(oss.str().length() > 0){
      ssize_t bytes_sent = send(sockfd, oss.str().c_str(), oss.str().length(), flags);
      if (bytes_sent == -1) {
         close(sockfd);
         return;
      }
      sum = sum + bytes_sent;
      oss.str(oss.str().substr(bytes_sent));
   }
   std::cout << "Total bytes sent: " << sum << std::endl;
}

void sendData_one_MB_sleep(int sockfd)
{
   std::string source = "This is a long string";
   int sum = 0;
   while(sum < 1024 * 1024){
      sleep(1);
      ssize_t bytes_sent = send(sockfd, source.c_str(), source.length(), 0);
      if (bytes_sent == -1) {
         close(sockfd);
         return;
      }
      sum = sum + bytes_sent;
   }
   std::cout << "Total bytes sent: " << sum << std::endl;
}

int CreateSocket_Connect(char *argv[], int argc)
{
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   char buffer[256];
   
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
	
   portno = atoi(argv[2]);
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	
   server = gethostbyname(argv[1]);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   
      // int flags = fcntl(sockfd, F_GETFL, 0); // Get current flags
		// 		flags |= O_NONBLOCK; // Set non-blocking flag
		// 		int result = fcntl(sockfd, F_SETFL, flags);
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
//    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }
   return sockfd;
}

int main(int argc, char *argv[]) {

   
   int sockfd = CreateSocket_Connect(argv, argc);
   int n = 0;
   char buffer[256];
   
   // for debug when client send data to server size 1 GB in machine school
   // sentDataHuge();
   // for debug when client send data to server size 1 MB in my machine
   // and use funtion sleep for hold the data a lot of time
   sendData_one_MB_sleep(sockfd);

   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
	
   printf("%s\n",buffer);
   return 0;
}