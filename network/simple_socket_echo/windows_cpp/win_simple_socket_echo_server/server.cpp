#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <cstring>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "19999"

void run_server(){
	WSADATA wsadata;
	int res;

	SOCKET sock_listen = INVALID_SOCKET;
	SOCKET sock_client = INVALID_SOCKET;

	struct addrinfo* resolved_addr = nullptr;
	struct addrinfo hints = { 0 };

	int recvres, sendres, bytesent;
	char recvbuf[DEFAULT_BUFLEN] = {0};
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	if ((res = WSAStartup(MAKEWORD(2,2), &wsadata)) != 0) {
		printf("WSAStartup failed with error: %d\n", res); return;
	}

	// do some initialization
	do{
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		if ( (res = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resolved_addr)) != 0 ) {
			printf("getaddrinfo failed with error: %d\n", res);
			break;
		}
		// Create a SOCKET for connecting to server
		sock_listen = socket(resolved_addr->ai_family, resolved_addr->ai_socktype, resolved_addr->ai_protocol);
		if (sock_listen == INVALID_SOCKET) {
			printf("socket failed with error: %d\n", WSAGetLastError());
			break;
		}
		// Setup the TCP listening socket
		if ((res = bind( sock_listen, resolved_addr->ai_addr, (int)resolved_addr->ai_addrlen)) == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			break;
		}
		if ((res = listen(sock_listen, SOMAXCONN)) == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			break;
		}

		// infinite accept loop
		while((sock_client = accept(sock_listen, NULL, NULL)) != INVALID_SOCKET){
			// Receive until the peer shuts down the connection
			while((recvres = recv(sock_client, recvbuf, recvbuflen, 0)) > 0){
				printf("recv(%d):%s\n", recvres, recvbuf);
				bytesent = 0;
				while(bytesent < recvres){
					// Echo the buffer back to the sender
					sendres = send( sock_client, recvbuf+bytesent, recvres-bytesent, 0 );
					if (sendres == SOCKET_ERROR) {
						printf("send failed with error: %d\n", WSAGetLastError());
						break;
					}
					bytesent += sendres;
				}
				printf("Bytes sent: %d\n", bytesent);
				if(bytesent < recvres)
					break;
				memset(recvbuf,0,sizeof(recvbuf));
			}

			if(recvres == 0){
				printf("graceful shutdown by client\n");
			}
			else if(recvres < 0){
				printf("recv failed with error: %d\n", WSAGetLastError());
			}

			// shutdown the connection since we're done
			shutdown(sock_client, SD_BOTH);
			closesocket(sock_client);
			sock_client = INVALID_SOCKET;
		}

	} while(0);

	if(resolved_addr)
		freeaddrinfo(resolved_addr);
	if(sock_listen != INVALID_SOCKET)
		closesocket(sock_listen);
	if(sock_client != INVALID_SOCKET)
		closesocket(sock_client);

	WSACleanup();
}


int main(int argc, char** argv){
	run_server();
	return 0;
}
