#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 8192

int setupServer(char* ipAddressString, char* portString);

int setupServer(char* ipAddressString, char* portString){
	int server = -1;
	//struct sockaddr_in needs netinet/in.h & arpa/inet.h.....
	struct sockaddr_in address;
	int port = atoi(portString);

	printf("Opening server %s:%d...\n", ipAddressString, port);

	if((server = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("setupServer socket failed\n");
		printf("errno: %d    errstr: %s\n", errno, strerror(errno));
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ipAddressString);
	address.sin_port = htons(port);

	if(bind(server, (struct sockaddr*)&address, sizeof(address)) == -1){
		printf("setupServer bind failed\n");
		printf("errno: %d    errstr: %s\n", errno, strerror(errno));
		exit(1);
	}

	if(listen(server, 5) == -1){
		printf("setupServer listen failed\n");
		printf("errno: %d    errstr: %s\n", errno, strerror(errno));
		exit(1);
	}
	return server;
}

void singleAcceptEcho(int server){
	int client;
	char buf[BUFSIZE] = {0};
	int readResult = 0;

	while((client = accept(server, NULL, 0)) != -1){
		while(1){
			readResult = read(client, buf, BUFSIZE);
			if(readResult == -1){
				printf("singleAcceptEcho read failed\n");
				printf("errno: %d    errstr: %s\n", errno, strerror(errno));
				exit(1);
			}
			if(readResult == 0){
				printf("client closed connection\n");
				break;
			}
			printf("%s\n", buf);
		}
	}

	if(client == -1){
		printf("singleAcceptEcho accept failed\n");
		printf("errno: %d    errstr: %s\n", errno, strerror(errno));
		exit(1);
	}
}

int main(int argc, char** argv){
	if(argc != 3){
		printf("Usage: %s <echo server ip> <echo server port>\n", argv[0]);
		exit(1);
	}

	int server = setupServer(argv[1], argv[2]);
	singleAcceptEcho(server);
}