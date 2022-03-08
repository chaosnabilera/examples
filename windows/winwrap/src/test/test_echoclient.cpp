#include <iostream>
#include <string>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

void run_echoclient(string server_addr, string server_port) {
	WinSock* conn = nullptr;
	WinTCP& wintcp = WinTCP::get_instance();
	string line;
	int linelen, recvlen;
	size_t sendcnt, recvcnt;
	char recvbuf[0x1000];

	do {
		if ((conn = wintcp.tcp_connect(server_addr, server_port, true)) == nullptr) {
			dprintf("wintcp.tcp_connect(%s,%s) failed", server_addr.c_str(), server_port.c_str());
			break;
		}
		printf("Connected to %s:%d\n", conn->get_ip().c_str(), conn->get_port());
		while (true) {
			printf(">>");
			getline(cin, line);
			linelen = line.size();
			if (!conn->send_all_block(sizeof(int), (char*)&linelen, sizeof(int), sendcnt)) {
				dprintf("send linelen failed");
				break;
			}
			if (!conn->send_all_block(line.size(), (char*)line.c_str(), line.size(), sendcnt)) {
				dprintf("send content failed");
				break;
			}
			if (!conn->recv_all_block(sizeof(int), (char*)&recvlen, sizeof(int), recvcnt)) {
				dprintf("recv recvlen failed");
				break;
			}
			if (!conn->recv_all_block(recvlen, recvbuf, sizeof(recvbuf), recvcnt)) {
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