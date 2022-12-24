#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <cstring>

#include "tcp_common.h"

static bool sWSAInitialized = false;

bool WSAInitialize() {
	WSADATA wsadata = { 0 };
	int lasterror = 0;

	if(!sWSAInitialized){
		if(WSAStartup(MAKEWORD(2,2), &wsadata) == ERROR_SUCCESS){
			sWSAInitialized = true;
		}
		else{
			lasterror = WSAGetLastError();
			printf("[WSAInitialize] WSAStartup failed : 0x%08x (%d)", lasterror, lasterror);
		}
	}

	return sWSAInitialized;
}

bool WSADeinitialize() {
	bool result = true; // when called without WSAInitialize, return true
	int lasterror = 0;

	if(sWSAInitialized){
		if(WSACleanup() == ERROR_SUCCESS){
			sWSAInitialized = false;
			result = true;
		}
		else{
			result = false;
			lasterror = WSAGetLastError();
			printf("[WSADeinitialize] WSACleanup failed : 0x%08x (%d)", lasterror, lasterror);
		}
	}
	return result;
}

bool TCPConnect(const std::string& server_addr, const std::string& server_port, SOCKET* out_sock_conn) {
	bool result = false;
	int ires = 0;
	int lasterror = 0;
	SOCKET sock_conn = INVALID_SOCKET;
	struct addrinfo* resolved_addr = nullptr;
	struct addrinfo* ptr = nullptr;
	struct addrinfo hints = { 0 };

	do {
		// Resolve the server address and port
		if((ires = getaddrinfo(server_addr.c_str(), server_port.c_str(), &hints, &resolved_addr)) != ERROR_SUCCESS){
			printf("[TCPConnect] getaddrinfo failed with error: %d\n", ires); 
			break;
		}	

		// Attempt to connect to an address until one succeeds
		for(ptr=resolved_addr; ptr != NULL ;ptr=ptr->ai_next) {
			// Create a SOCKET for connecting to server
			sock_conn = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (sock_conn == INVALID_SOCKET) {
				lasterror = WSAGetLastError();
				printf("[TCPConnect] socket failed with error: 0x%08x(%d)\n", lasterror, lasterror);
				break;
			}
			// Connect to server.
			if ((ires = connect( sock_conn, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR) {
				closesocket(sock_conn);
				sock_conn = INVALID_SOCKET;
				continue;
			}

			// success
			result = true;
			break;
		}

		if (sock_conn == INVALID_SOCKET) {
			printf("[TCPConnect] Unable to connect to server!\n");
			break;
		}
	} while(0);

	if(resolved_addr != nullptr){
		freeaddrinfo(resolved_addr);
	}
	if(result){
		*out_sock_conn = sock_conn;
	}

	return result;
}

bool TCPCreateListener(const std::string& server_port, SOCKET* out_sock_listen) {
	bool result = false;
	int ires = 0;
	int lasterror = 0;
	SOCKET sock_listen = INVALID_SOCKET;

	struct addrinfo* resolved_addr = nullptr;
	struct addrinfo hints = { 0 };

	do {
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		if ( (ires = getaddrinfo(NULL, server_port.c_str(), &hints, &resolved_addr)) != 0 ) {
			printf("[TCPCreateListener] getaddrinfo failed with error: %d\n", ires);
			break;
		}
		// Create a SOCKET for connecting to server
		sock_listen = socket(resolved_addr->ai_family, resolved_addr->ai_socktype, resolved_addr->ai_protocol);
		if (sock_listen == INVALID_SOCKET) {
			lasterror = WSAGetLastError();
			printf("[TCPCreateListener] socket failed with error: 0x%08x(%d)\n", lasterror, lasterror);
			break;
		}
		// Setup the TCP listening socket
		if ((ires = bind(sock_listen, resolved_addr->ai_addr, (int)resolved_addr->ai_addrlen)) == SOCKET_ERROR) {
			lasterror = WSAGetLastError();
			printf("[TCPCreateListener] bind failed with error: 0x%08x(%d)\n", lasterror, lasterror);
			break;
		}
		if ((ires = listen(sock_listen, SOMAXCONN)) == SOCKET_ERROR) {
			lasterror = WSAGetLastError();
			printf("[TCPCreateListener] listen failed with error: 0x%08x(%d)\n", lasterror, lasterror);
			break;
		}

		result = true;
	} while(0);

	if(resolved_addr != nullptr){
		freeaddrinfo(resolved_addr);
	}
	if(result){
		*out_sock_listen = sock_listen;
	}
	else{
		if(sock_listen != INVALID_SOCKET){
			closesocket(sock_listen);
			sock_listen = INVALID_SOCKET;
		}
	}

	return result;
}

bool TCPAccept(SOCKET sock_listen, SOCKET* out_sock_client) {
	bool result = false;
	SOCKET sock_accept = INVALID_SOCKET;
	sock_accept = accept(sock_listen, NULL, NULL);

	if(sock_accept != INVALID_SOCKET){
		result = true;
		*out_sock_client = sock_accept;
	}

	return result;
}