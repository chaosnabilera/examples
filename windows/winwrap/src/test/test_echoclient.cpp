#include <iostream>
#include <string>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

void run_echoclient_blockio(string server_addr, string server_port) {
	WinSock* conn = nullptr;
	WinTCP& wintcp = WinTCP::get_instance();
	string line;
	int linelen, recvlen;
	size_t sendcnt, recvcnt;
	char recvbuf[0x1000];

	do {
		if ((conn = wintcp.tcp_connect(server_addr, server_port, true)) == nullptr) {
			dprintf("[run_echoclient_blockio] wintcp.tcp_connect(%s,%s) failed", server_addr.c_str(), server_port.c_str());
			break;
		}
		printf("Connected to %s:%d\n", conn->get_ip().c_str(), conn->get_port());
		while (true) {
			printf(">>");
			getline(cin, line);
			linelen = line.size();
			if (!conn->send_all_block(sizeof(int), (char*)&linelen, sizeof(int), sendcnt)) {
				dprintf("[run_echoclient_blockio] send linelen failed");
				break;
			}
			if (!conn->send_all_block(line.size(), (char*)line.c_str(), line.size(), sendcnt)) {
				dprintf("[run_echoclient_blockio] send content failed");
				break;
			}
			if (!conn->recv_all_block(sizeof(int), (char*)&recvlen, sizeof(int), recvcnt)) {
				dprintf("[run_echoclient_blockio] recv recvlen failed");
				break;
			}
			if (!conn->recv_all_block(recvlen, recvbuf, sizeof(recvbuf), recvcnt)) {
				dprintf("[run_echoclient_blockio] recv content failed");
				break;
			}
			recvbuf[recvlen] = '\0';
			printf("recv(%d):%s\n", recvlen, recvbuf);
		}
	} while (0);

	if (conn) {
		delete conn;
	}
}

void run_echoclient_nonblockio(string server_addr, string server_port) {
	WinSock* conn = nullptr;
	WinTCP& wintcp = WinTCP::get_instance();
	string line;
	int linelen, recvlen;
	size_t sendcnt, recvcnt;
	char recvbuf[0x1000];
	const int timeout_ms = 1000; // 1sec

	do {
		if ((conn = wintcp.tcp_connect(server_addr, server_port, false)) == nullptr) {
			dprintf("[run_echoclient_nonblockio] wintcp.tcp_connect(%s,%s) failed", server_addr.c_str(), server_port.c_str());
			break;
		}
		printf("Connected to %s:%d\n", conn->get_ip().c_str(), conn->get_port());
		while (true) {
			printf(">>");
			getline(cin, line);
			linelen = line.size();
			if (!conn->send_all_nonblock(sizeof(int), (char*)&linelen, sizeof(int), timeout_ms, sendcnt)) {
				dprintf("send linelen failed");
				break;
			}
			if (!conn->send_all_nonblock(line.size(), (char*)line.c_str(), line.size(), timeout_ms, sendcnt)) {
				dprintf("send content failed");
				break;
			}
			if (!conn->recv_all_nonblock(sizeof(int), (char*)&recvlen, sizeof(int), timeout_ms, recvcnt)) {
				dprintf("recv recvlen failed");
				break;
			}
			if (!conn->recv_all_nonblock(recvlen, recvbuf, sizeof(recvbuf), timeout_ms, recvcnt)) {
				dprintf("recv content failed");
				break;
			}
			recvbuf[recvlen] = '\0';
			printf("recv(%d):%s\n", recvlen, recvbuf);
		}
	} while (0);

	if (conn) {
		delete conn;
	}
}