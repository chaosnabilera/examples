#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

unique_ptr<WinTCP> WinTCP::instance;
once_flag WinTCP::mOnceFlag;

WinTCP& WinTCP::getInstance() {
	call_once(WinTCP::mOnceFlag, []() {
		instance.reset(new WinTCP());
	});
	return *(instance.get());
}

WinTCP::WinTCP() {
	WSADATA wsadata{ 0 };
	DWORD wsa_startup_result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (!(wsaStartupSuccess = (wsa_startup_result == ERROR_SUCCESS))) {
		dprintf("WSAStartup failed : %d", wsa_startup_result);
	}
}

WinTCP::~WinTCP() {
	if (wsaStartupSuccess) {
		WSACleanup();
	}
}

WinSock* WinTCP::tcpConnect(string server_addr, string service_port, bool blocking) {
	SOCKET sock_conn = INVALID_SOCKET;
	WinSock* winsock = nullptr;

	struct addrinfo* res_addr = nullptr;
	struct addrinfo hints = { 0 };
	char client_addr[sizeof(sockaddr_in6)] = { 0 };

	int ires = 0;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	do {
		// Resolve the server address and port
		if ((ires = getaddrinfo(server_addr.c_str(), service_port.c_str(), &hints, &res_addr)) != ERROR_SUCCESS) {
			dprintf("getaddrinfo failed with error: %d\n", ires);
			break;
		}

		// Attempt to connect to an address until one succeeds
		for (struct addrinfo* ptr = res_addr; ptr != nullptr; ptr = ptr->ai_next) {
			// Create a SOCKET for connecting to server
			sock_conn = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (sock_conn == INVALID_SOCKET) {
				dprintf("socket failed with error: %d\n", WSAGetLastError());
				break;
			}
			// Connect to server.
			if ((ires = connect(sock_conn, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR) {
				closesocket(sock_conn);
				sock_conn = INVALID_SOCKET;
				continue;
			}
			memcpy(client_addr, ptr->ai_addr, ptr->ai_addrlen);
			break;
		}

		if (sock_conn == INVALID_SOCKET) {
			dprintf("Unable to connect to server!\n");
			break;
		}
	} while (0);

	if (res_addr)
		freeaddrinfo(res_addr);

	if (sock_conn != INVALID_SOCKET) {
		winsock = new WinSock(sock_conn, (sockaddr*)client_addr, blocking);
	}

	return winsock;
}

WinAcceptSock* WinTCP::tcpCreateListener(std::string server_addr, std::string service_port, bool blocking) {
	SOCKET sock_listen = INVALID_SOCKET;
	WinAcceptSock* winsock = nullptr;
	struct addrinfo* res_addr = nullptr;
	struct addrinfo hints = { 0 };

	int ires = 0;
	bool success = false;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	do {
		// Resolve the server address and port
		if ((ires = getaddrinfo(server_addr.c_str(), service_port.c_str(), &hints, &res_addr)) != 0) {
			printf("[tcp_create_listener] getaddrinfo failed with error: %d\n", ires);
			break;
		}
		// Create a SOCKET for connecting to server
		sock_listen = socket(res_addr->ai_family, res_addr->ai_socktype, res_addr->ai_protocol);
		if (sock_listen == INVALID_SOCKET) {
			printf("socket failed with error: %d\n", WSAGetLastError());
			break;
		}
		// Setup the TCP listening socket
		if ((ires = bind(sock_listen, res_addr->ai_addr, (int)res_addr->ai_addrlen)) == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			break;
		}
		if ((ires = listen(sock_listen, SOMAXCONN)) == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			break;
		}
		
		success = true;

	} while (0);

	if (res_addr)
		freeaddrinfo(res_addr);

	if (!success && sock_listen != INVALID_SOCKET) {
		closesocket(sock_listen); 
		sock_listen = INVALID_SOCKET;
	}

	if (sock_listen != INVALID_SOCKET) {
		winsock = new WinAcceptSock(sock_listen, blocking);
	}

	return winsock;
}



