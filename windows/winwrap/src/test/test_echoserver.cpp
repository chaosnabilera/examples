#include <iostream>
#include <string>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

void run_echoserver_blockio(string server_addr, string server_port) {
	WinTCP& wintcp = WinTCP::get_instance();
	WinAcceptSock* as = nullptr;
	WinSock* s = nullptr;

	int recvlen;
	size_t recvcnt, sendcnt;
	char recvbuf[0x1000];

	do {
		if ((as = wintcp.tcp_create_listener(server_addr, server_port, true)) == nullptr) {
			dprintf("wintcp.tcp_create_listener(%s,%s) failed", server_addr.c_str(), server_port.c_str());
			break;
		}
		while ((s = as->accept_block()) != nullptr) {
			printf("Connection from %s:%d\n", s->get_ip().c_str(), s->get_port());
			while (true) {
				if (!s->recv_all_block(sizeof(int), (char*)&recvlen, sizeof(int), recvcnt)) {
					dprintf("recv recvlen failed");
					break;
				}
				if (!s->recv_all_block(recvlen, recvbuf, sizeof(recvbuf), recvcnt)) {
					dprintf("recv content failed");
					break;
				}
				recvbuf[recvlen] = '\0';
				printf("recv(%d): %s\n", recvlen, recvbuf);
				if (!s->send_all_block(sizeof(int), (char*)&recvlen, sizeof(int), sendcnt)) {
					dprintf("send recvlen failed");
					break;
				}
				if (!s->send_all_block(recvlen, recvbuf, sizeof(recvbuf), sendcnt)) {
					dprintf("send content failed");
					break;
				}
			}
			delete s;
		}
	} while (0);
}

void run_echoserver_nonblockio(string server_addr, string server_port) {
	WinTCP& wintcp = WinTCP::get_instance();
	WinAcceptSock* as = nullptr;
	WinSock* s = nullptr;
	const int accept_poll_period_ms = 10;

	int recvlen;
	size_t recvcnt, sendcnt;
	char recvbuf[0x1000];
	int timeout_msgstart = 60*1000; // wait 1min for start of message
	int timeout_ms = 1000;

	do {
		if ((as = wintcp.tcp_create_listener(server_addr, server_port, false)) == nullptr) {
			dprintf("wintcp.tcp_create_listener(%s,%s) failed", server_addr.c_str(), server_port.c_str());
			break;
		}
		while (as->accept_nonblock(s)) {
			if (s == nullptr) {
				Sleep(accept_poll_period_ms);
				continue;
			}
			printf("Connection from %s:%d\n", s->get_ip().c_str(), s->get_port());
			while (true) {
				if (!s->recv_all_nonblock(sizeof(int), (char*)&recvlen, sizeof(int), timeout_msgstart, recvcnt)) {
					dprintf("recv recvlen failed");
					break;
				}
				if (!s->recv_all_nonblock(recvlen, recvbuf, sizeof(recvbuf), timeout_ms, recvcnt)) {
					dprintf("recv content failed");
					break;
				}
				recvbuf[recvlen] = '\0';
				printf("recv(%d): %s\n", recvlen, recvbuf);
				if (!s->send_all_nonblock(sizeof(int), (char*)&recvlen, sizeof(int), timeout_ms, sendcnt)) {
					dprintf("send recvlen failed");
					break;
				}
				if (!s->send_all_nonblock(recvlen, recvbuf, sizeof(recvbuf), timeout_ms, sendcnt)) {
					dprintf("send content failed");
					break;
				}
			}
			delete s;
		}
	} while (0);
}