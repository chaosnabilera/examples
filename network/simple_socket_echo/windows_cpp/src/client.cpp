#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <cstring>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "19999"

void echo_client(char* server_addr, char* server_port){
	WSADATA wsadata;
	SOCKET sock_conn = INVALID_SOCKET;
	struct addrinfo* resolved_addr = nullptr;
	struct addrinfo* ptr = nullptr;
	struct addrinfo hints = { 0 };

	char sendbuf[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	int res;
	int recvbuflen = DEFAULT_BUFLEN;
	int sendlen = 0;
	int sendres, recvres, bytesent, byterecv;
	bool keep_running = true;

	if((res = WSAStartup(MAKEWORD(2,2), &wsadata)) != ERROR_SUCCESS){
		printf("WSAStartup failed with error: %d\n", res);
		return;
	}

	do{
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		if((res = getaddrinfo(server_addr, server_port, &hints, &resolved_addr)) != ERROR_SUCCESS){
			printf("getaddrinfo failed with error: %d\n", res); break;
		}		

		// Attempt to connect to an address until one succeeds
		for(ptr=resolved_addr; ptr != NULL ;ptr=ptr->ai_next) {
			// Create a SOCKET for connecting to server
			sock_conn = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (sock_conn == INVALID_SOCKET) {
				printf("socket failed with error: %d\n", WSAGetLastError());
				break;
			}
			// Connect to server.
			if ((res = connect( sock_conn, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR) {
				closesocket(sock_conn);
				sock_conn = INVALID_SOCKET;
				continue;
			}
			break;
		}

		if (sock_conn == INVALID_SOCKET) {
			printf("Unable to connect to server!\n");
			break;
		}

		while(true){
			printf(">>");
			scanf("%s", sendbuf);
			sendlen = strlen(sendbuf);
			bytesent = 0;
			while(bytesent < sendlen){
				// Send an initial buffer
				if((sendres = send( sock_conn, sendbuf + bytesent, sendlen - bytesent, 0 )) == SOCKET_ERROR){
					printf("send failed with error: %d\n", WSAGetLastError());
					keep_running = false;
					break;
				}
				bytesent += sendres;
			}
			printf("Bytes Sent: %d\n", bytesent);

			if(!keep_running)
				break;

			recvres = recv(sock_conn, recvbuf, recvbuflen, 0);
			if ( recvres > 0 ){
				recvbuf[recvres] = 0;
				printf("recv(%d)> %s\n", recvres, recvbuf);
			}
			else if ( recvres == 0 ){
				printf("graceful shutdown by server\n");
				break;
			}
			else{
				printf("recv failed with error: %d\n", WSAGetLastError());
				break;
			}
		}

	} while(0);

	if(resolved_addr != nullptr)
		freeaddrinfo(resolved_addr);
	if(sock_conn != INVALID_SOCKET){
		closesocket(sock_conn);
	}
	WSACleanup();
	
}

int main(int argc, char **argv)
{
	if(argc == 3){
		printf("server: %s\n",argv[1]);
		printf("port: %s\n", argv[2]);
		echo_client(argv[1], argv[2]);	
	}
	else{
		printf("usage: %s <server> <port>\n",argv[0]);
	}
}
