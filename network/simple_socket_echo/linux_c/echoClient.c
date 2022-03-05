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

int makeConnection(char* ipAddressString, char* portString);

int makeConnection(char* ipAddressString, char* portString){
	int conn = -1;
	//struct sockaddr_in needs netinet/in.h & arpa/inet.h.....
	struct sockaddr_in address;
	int port = atoi(portString);

	printf("Making connection to %s:%d\n", ipAddressString, port);

	if((conn = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("makeConnection socket failed\n");
		printf("errno: %d    errstr: %s\n", errno, strerror(errno));
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ipAddressString);
	address.sin_port = htons(port);

	if(connect(conn, (struct sockaddr*)&address, sizeof(address)) == -1){
		printf("makeConnection connect failed\n");
		printf("errno: %d    errstr: %s\n", errno, strerror(errno));
		exit(1);
	}

	printf("Success\n");

	return conn;
}

int main(int argc, char** argv){

	if(argc != 3){
		printf("Usage: %s <echo server ip> <echo server port>\n", argv[0]);
		exit(1);
	}

	int sock = makeConnection(argv[1], argv[2]);
	char buf[BUFSIZE] = {0};

	while(1){
		printf(">");
		if(fgets(buf, BUFSIZE, stdin) == NULL){
			printf("fgets error: %d :%s\n",errno, strerror(errno));
			exit(1);
		}
		//Remove \n at the end
		buf[strlen(buf)-1] = '\x00';
		write(sock, buf, BUFSIZE);
	}
}