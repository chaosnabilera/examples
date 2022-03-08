#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

WinAcceptSock::WinAcceptSock(SOCKET _sock, bool _is_blocking) {
	sock = _sock;
	is_blocking = _is_blocking;

	if (sock == INVALID_SOCKET)
		return;
	if(!switch_blocking_mode(is_blocking, true))
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
			dprintf("[accept_block] Tried accept_block on INVALID_SOCKET");
			break;
		}
		if (!switch_blocking_mode(true)) {
			dprintf("[accept_block] Failed to change socket to blocking mode");
			break;
		}
		if ((sock_client = accept(sock, &client_addr, &client_addr_len)) == INVALID_SOCKET) {
			dprintf("[accept_block] accept failed with error: %d", WSAGetLastError());
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
		if (!switch_blocking_mode(false)) {
			dprintf("Failed to change socket to non-blocking mode");
			break;
		}
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