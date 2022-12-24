#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Winsock2.h>
#include <ws2tcpip.h>

#include "tcp_common.h"

#define DEFAULT_BUFLEN 4096

void sRunEchoServer(const std::string& server_port);
void sHandleConnection(SOCKET sock);

void sHandleConnection(SOCKET sock){
	int lasterror = 0;
	char recvbuf[DEFAULT_BUFLEN] = {0};
	int recvres, sendres, bytesent;

	// Receive until the peer shuts down the connection
	while((recvres = recv(sock, recvbuf, sizeof(recvbuf), 0)) > 0){
		printf("[sHandleConnection] recv(%d):%s\n", recvres, recvbuf);

		bytesent = 0;
		while(bytesent < recvres){
			// Echo the buffer back to the sender
			sendres = send( sock, recvbuf+bytesent, recvres-bytesent, 0 );
			if (sendres == SOCKET_ERROR) {
				lasterror = WSAGetLastError();
				printf("[sHandleConnection] send failed with error: 0x%08x(%d)\n", lasterror, lasterror);
				break;
			}
			bytesent += sendres;
		}
		printf("[sHandleConnection] Bytes sent: %d\n", bytesent);
		if(bytesent < recvres)
			break;
		memset(recvbuf,0,sizeof(recvbuf));
	}

	if(recvres == 0) {
		printf("[sHandleConnection] graceful shutdown by client\n");
	}
	else {
		lasterror = WSAGetLastError();
		printf("[sHandleConnection] recv failed with error: 0x%08x(%d)\n", lasterror, lasterror);
	}
}

void sRunEchoServer(const std::string& server_port){
	SOCKET sock_listen = INVALID_SOCKET;
	SOCKET sock_client = INVALID_SOCKET;

	// do some initialization
	do{
		if(!TCPCreateListener(server_port, &sock_listen)){
			printf("[sRunEchoServer] TCPCreateListener failed");
			break;
		}
		
		// infinite accept loop
		while(TCPAccept(sock_listen, &sock_client)){
			sHandleConnection(sock_client);
			shutdown(sock_client, SD_BOTH);
			closesocket(sock_client);
			sock_client = INVALID_SOCKET;
		}
		closesocket(sock_listen);
	} while(0);
}


int main(int argc, char** argv){
	std::string server_port;

	if(argc == 2){
		server_port = argv[1];
		if(!WSAInitialize()){
			printf("[main] WSAInitialize failed");
			return 1;
		}

		sRunEchoServer(server_port);
		
		if(!WSADeinitialize()){
			printf("[main] WSADeinitialize failed");
			return 1;
		}
	}
	else{
		printf("usage: %s <port> (host ip == 0.0.0.0)\n", argv[0]);
	}
	
	return 0;
}
