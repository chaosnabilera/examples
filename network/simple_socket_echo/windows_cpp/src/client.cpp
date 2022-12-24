#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Winsock2.h>
#include <ws2tcpip.h>

#include "tcp_common.h"

static const unsigned int DEFAULT_BUFLEN = 0x1000;

static void sEchoClient(const std::string& server_addr, const std::string& server_port);

void sEchoClient(const std::string& server_addr, const std::string& server_port){
	SOCKET sock = INVALID_SOCKET;
	char sendbuf[DEFAULT_BUFLEN] = { 0 };
	char recvbuf[DEFAULT_BUFLEN] = { 0 };
	int lasterror = 0;
	int sendlen = 0;
	int sendres, recvres, bytesent;
	bool is_connected = false;

	do {
		if(!TCPConnect(server_addr, server_port, &sock)){
			printf("[sEchoClient] TCPConnect failed\n");
			break;
		}		

		is_connected = true;

		while(is_connected){
			printf(">>");
			scanf_s("%s", sendbuf, DEFAULT_BUFLEN);

			sendlen = (int)(strlen(sendbuf) + 1);
			bytesent = 0;
			while(bytesent < sendlen){
				// Send an initial buffer
				if((sendres = send( sock, sendbuf + bytesent, sendlen - bytesent, 0 )) == SOCKET_ERROR) {
					lasterror = WSAGetLastError();
					printf("[sEchoClient] send failed with error: 0x%08x (%d)\n", lasterror, lasterror);
					is_connected = false;
					break;
				}
				bytesent += sendres;
			}

			printf("[sEchoClient] Bytes Sent: %d\n", bytesent);

			if(!is_connected){
				break;
			}

			recvres = recv(sock, recvbuf, sizeof(recvbuf), 0);
			if ( recvres > 0 ){
				printf("[sEchoClient] recv(%d)> %s\n", recvres, recvbuf);
			}
			else if ( recvres == 0 ){
				printf("[sEchoClient] graceful shutdown by server\n");
				is_connected = false;
				break;
			}
			else{
				lasterror = WSAGetLastError();
				printf("[sEchoClient] recv failed with error: 0x%08x (%d)\n", lasterror, lasterror);
				is_connected = false;
				break;
			}
		}
	} while(0);
	
	if(sock != INVALID_SOCKET){
		closesocket(sock);
	}	
}

int main(int argc, char **argv) {
	std::string server_addr;
	std::string server_port;

	if(argc == 3){
		server_addr = argv[1];
		server_port = argv[2];

		if(!WSAInitialize()){
			printf("[main] WSAInitialize failed");
			return 1;
		}

		sEchoClient(server_addr, server_port);
		
		if(!WSADeinitialize()){
			printf("[main] WSADeinitialize failed");
			return 1;
		}
	}
	else{
		printf("usage: %s <server> <port>\n", argv[0]);
	}

	return 0;
}
