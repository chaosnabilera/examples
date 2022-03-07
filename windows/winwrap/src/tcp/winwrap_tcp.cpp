#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

static bool set_socket_blocking_mode(SOCKET sock, bool blocking);

static bool set_socket_blocking_mode(SOCKET sock, bool blocking) {
	u_long mode = blocking ? 0 : 1;
	bool res = (ioctlsocket(sock, FIONBIO, &mode) == NO_ERROR);
	dprintf("set_socket_blocking_mode(%p, %d) failed", sock, blocking);
	return res;
}

WinTCP& WinTCP::get_instance() {
	call_once(WinTCP::m_onceflag, []() {
		instance.reset(new WinTCP());
	});
	return *(instance.get());
}

WinTCP::WinTCP() {
	WSADATA wsadata{ 0 };
	DWORD wsa_startup_result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (!(wsa_startup_success = (wsa_startup_result == ERROR_SUCCESS))) {
		dprintf("WSAStartup failed : %d", wsa_startup_result);
	}
}

WinTCP::~WinTCP() {
	if (wsa_startup_success) {
		WSACleanup();
	}
}

WinSock* WinTCP::tcp_connect(string server_addr, string service_port, bool blocking) {
	SOCKET sock_conn = INVALID_SOCKET;
	WinSock* winsock = nullptr;

	struct addrinfo* res_addr = nullptr;
	struct addrinfo hints = { 0 };
	sockaddr client_addr = { 0 };

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
			client_addr = *(ptr->ai_addr);
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
		winsock = new WinSock(sock_conn, client_addr ,blocking);
	}

	return winsock;
}

WinAcceptSock* WinTCP::tcp_create_listener(std::string server_addr, std::string service_port, bool blocking) {
	SOCKET sock_listen = INVALID_SOCKET;
	WinAcceptSock* winsock = nullptr;
	struct addrinfo* res_addr = nullptr;
	struct addrinfo hints = { 0 };

	int ires = 0;
	bool success = false;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	do {
		// Resolve the server address and port
		if ((ires = getaddrinfo(server_addr.c_str(), service_port.c_str(), &hints, &res_addr)) != 0) {
			printf("getaddrinfo failed with error: %d\n", ires);
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

WinAcceptSock::WinAcceptSock(SOCKET _sock, bool _is_blocking) : sock(_sock), is_blocking(_is_blocking) { 
	if (sock == INVALID_SOCKET)
		return;
	if (!set_socket_blocking_mode(sock, is_blocking))
		close();
}

WinAcceptSock::~WinAcceptSock() {
	close();
}

WinSock* WinAcceptSock::accept_block() {
	SOCKET sock_client = INVALID_SOCKET;
	WinSock* winsock = nullptr;
	bool success = false;
	sockaddr client_addr = { 0 };
	int client_addr_len = sizeof(sockaddr);

	do {
		if (sock == INVALID_SOCKET) {
			dprintf("Tried accept_block on INVALID_SOCKET");
			break;
		}

		if (!is_blocking && !set_socket_blocking_mode(sock, true)) {
			dprintf("Failed to change socket to blocking mode");
			break;
		}
		is_blocking = true;

		if ((sock_client = accept(sock, &client_addr, &client_addr_len)) == INVALID_SOCKET) {
			dprintf("accept failed with error: %d", WSAGetLastError());
			break;
		}
		success = true;
	} while (0);

	if (success) {
		winsock = new WinSock(sock_client, client_addr, true);
	}
	else {
		close();
	}

	return winsock;
}

bool WinAcceptSock::accept_nonblock(WinSock*& res) {
	SOCKET sock_client = INVALID_SOCKET;
	WinSock* winsock = nullptr;
	bool success = false;
	sockaddr client_addr = { 0 };
	int client_addr_len = sizeof(sockaddr);
	DWORD err = 0;

	do {
		if (sock == INVALID_SOCKET) {
			dprintf("Tried accept_block on INVALID_SOCKET");
			break;
		}

		if (is_blocking && !set_socket_blocking_mode(sock, false)) {
			dprintf("Failed to change socket to non-blocking mode");
			break;
		}
		is_blocking = false;

		if ((sock_client = accept(sock, &client_addr, &client_addr_len)) == INVALID_SOCKET) {
			if ((err = WSAGetLastError()) != WSAEWOULDBLOCK) {
				dprintf("Failed to accept with error other than WSAEWOULDBLOCK : %d", err);
				break;
			}
		}
		success = true;
	} while (0);

	if (success) {
		if (sock_client != INVALID_SOCKET)
			res = new WinSock(sock_client, client_addr, false);
		else
			res = nullptr;
	}
	else {
		close();
	}

	return success;
}

void WinAcceptSock::close() {
	if (sock == INVALID_SOCKET)
		return;
	closesocket(sock);
	sock = INVALID_SOCKET;
}

WinSock::WinSock(SOCKET _sock, sockaddr _addr, bool _is_blocking) : 
	sock(_sock), addr(_addr), is_blocking(_is_blocking) {
	if (sock == INVALID_SOCKET)
		return;
	if (!set_socket_blocking_mode(sock, is_blocking))
		close();
}

WinSock::~WinSock() {
	close();
}

void WinSock::close() {
	if (sock == INVALID_SOCKET)
		return;
	closesocket(sock);
	sock = INVALID_SOCKET;
}

bool WinSock::can_read() {
	int sres;
	TIMEVAL immediate = { 0,0 };
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	if (sock == INVALID_SOCKET)
		return false;

	if ((sres = select(0, &fds, nullptr, nullptr, &immediate)) == SOCKET_ERROR) {
		dprintf("Error occurred during can_read select");
		close();
	}

	return (sres > 0);
}

bool WinSock::can_write() {
	int sres;
	TIMEVAL immediate = { 0,0 };
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	if (sock == INVALID_SOCKET)
		return false;

	if ((sres = select(0, nullptr, &fds, nullptr, &immediate)) == SOCKET_ERROR) {
		dprintf("Error occurred during can_write select");
		close();
	}

	return (sres > 0);
}