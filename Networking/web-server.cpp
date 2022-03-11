#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <filesystem>
#include <sys/stat.h>
using namespace std;

#define PORT "8080"
#define BACKLOG 10

void sigchld_handler(int s) {
	//waitpid() might overwrite errno, so we save and restore it
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {

	if(argc != 3){
		fprintf(stderr, "usage: ./web-server [docroot] [port]\n");
		return 1;
	}
	string docRoot = argv[1];
	string port = argv[2];

	cout << "Doc root is " << docRoot << endl;
	cout << "Port number is " << port << endl;


	int sockfd, new_fd;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}


	for(p = servinfo; p !=  NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("server: socket");
			continue;
		}

		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}


	freeaddrinfo(servinfo);

	if(p == NULL){
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if(listen(sockfd, BACKLOG) == -1){
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if(sigaction(SIGCHLD, &sa, NULL) == -1){
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1){
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if(new_fd == -1){
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);

		if(!fork()){
			char dataBuf[1024];
			string filePath = "";
			filePath.append(docRoot);
			filePath.append("/");
			//
			//string successRequestResponse = "HTTP/1.1 200 OK\r\nDate: Mon, Mar 7 2022 2:24:53 GMT\r\nContent-Length: 27\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html><p>TEST</p></html>";
			string failRequestResponse = "HTTP/1.1 404";
			close(sockfd);
			while(1){
			int data = recv(new_fd, dataBuf, 1024, 0);
			string info = "";

			if(data == -1){
				perror("recv");
			}
			for(int i = 0; i < data; i++){
				info += dataBuf[i];
			}
			if(info.substr(0,3) == "GET"){
				
				int findSpace = info.find(" ", 5);
				filePath.append(info.substr(5, findSpace-5));
			}
			if(info.substr(0,4) == "HEAD"){
				ifstream file(filePath.c_str(), ifstream::in | ifstream::binary);
				file.seekg(0, ios::end);
				int fileSize = file.tellg();

				time_t rawTime;
				struct tm * timeInfo;
				char dateBuf[30];
				time (&rawTime);
				timeInfo = localtime (&rawTime);
				strftime(dateBuf, 30, "%a, %b %Y %T %Z", timeInfo);

				string successRequestResponse = "";
				successRequestResponse.append("HTTP/1.1 200 OK\r\nDate: ");
				successRequestResponse.append(dateBuf);
				successRequestResponse.append("\r\n");
				successRequestResponse.append("Content-Length: ");
				successRequestResponse.append(to_string (fileSize));
				successRequestResponse.append("\r\n");
				send(new_fd, successRequestResponse.c_str(), successRequestResponse.size(), 0);

			}
			if(info.substr(info.size()-4) == "\r\n\r\n"){
				time_t rawTime;
				struct tm * timeInfo;
				char dateBuf[30];
				time (&rawTime);
				timeInfo = localtime (&rawTime);
				strftime(dateBuf, 30, "%a, %b %Y %T %Z", timeInfo);
				failRequestResponse = "HTTP/1.1 404 Not Found\r\n";
				failRequestResponse.append(dateBuf);
				failRequestResponse.append("\r\n");
				failRequestResponse.append("Connection: Closed\r\n");
				fstream inFile;
				cout << "File Path: " << filePath << "\n";
				inFile.open(filePath, ios::in);
				if(!inFile.is_open()){
					cout << "Couldn't open file" << "\n";
					send(new_fd, failRequestResponse.c_str(), failRequestResponse.size(), 0);
					cout << failRequestResponse;
					break;
				}

				//Get size of the opened file
				ifstream file(filePath.c_str(), ifstream::in | ifstream::binary);
				file.seekg(0, ios::end);
				int fileSize = file.tellg();

				//Find the file type by checking the file extension
				int fileTypeOffset = filePath.find(".");
				string fileType = filePath.substr(fileTypeOffset+1, filePath.size());


				//This is very hacky, but it works ¯\_(ツ)_/¯ 
				string successRequestResponse = "";
				successRequestResponse.append("HTTP/1.1 200 OK\r\nDate: ");
				successRequestResponse.append(dateBuf);
				successRequestResponse.append("\r\n");
				successRequestResponse.append("Content-Length: ");
				successRequestResponse.append(to_string (fileSize));
				successRequestResponse.append("\r\n");
				if(fileType == "html" || fileType == "txt" || fileType == "css"){
					successRequestResponse.append("Content-Type: text/");
				}
				else if(fileType == "png" || fileType == "jpeg" || fileType == "gif"){
					successRequestResponse.append("Content-Type: image/");
				}
				else if(fileType == "pdf" || fileType == "js"){
					successRequestResponse.append("Content-Type: application/");
				}
				if(fileType == "txt"){
					successRequestResponse.append("plain");
				}
				else{
					successRequestResponse.append(fileType);
				}
				successRequestResponse.append("\r\n");
				successRequestResponse.append("Connection: Closed\r\n\r\n");	
				cout << successRequestResponse;
						send(new_fd, successRequestResponse.c_str(), successRequestResponse.size(), 0);
				while(1){
					char fileData[1024];
					if(!inFile.read(fileData, 1024)){
						send(new_fd, fileData, inFile.gcount(), 0);
						break;
					}
					cout << fileData << "\n";
					send(new_fd, fileData, inFile.gcount(), 0);
				}
				break;
			}
			}
			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}

	return 0;
}
