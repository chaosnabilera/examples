#include <iostream>
#include <string>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

void run_echoclient_blockio(string server_addr, string server_port) {
    WinSock* conn = nullptr;
    WinTCP& wintcp = WinTCP::getInstance();
    string line;
    int linelen, recvlen;
    size_t sendcnt, recvcnt;
    char recvbuf[0x1000];

    do {
        if ((conn = wintcp.tcpConnect(server_addr, server_port, true)) == nullptr) {
            dprintf("[run_echoclient_blockio] wintcp.tcp_connect(%s,%s) failed", server_addr.c_str(), server_port.c_str());
            break;
        }
        printf("Connected to %s:%d\n", conn->getIP().c_str(), conn->getPort());
        while (true) {
            printf(">>");
            getline(cin, line);
            linelen = line.size();
            if (!conn->sendAllBlock(sizeof(int), (char*)&linelen, sizeof(int), sendcnt)) {
                dprintf("[run_echoclient_blockio] send linelen failed");
                break;
            }
            if (!conn->sendAllBlock(line.size(), (char*)line.c_str(), line.size(), sendcnt)) {
                dprintf("[run_echoclient_blockio] send content failed");
                break;
            }
            if (!conn->recvAllBlock(sizeof(int), (char*)&recvlen, sizeof(int), recvcnt)) {
                dprintf("[run_echoclient_blockio] recv recvlen failed");
                break;
            }
            if (!conn->recvAllBlock(recvlen, recvbuf, sizeof(recvbuf), recvcnt)) {
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
    WinTCP& wintcp = WinTCP::getInstance();
    string line;
    int linelen, recvlen;
    size_t sendcnt, recvcnt;
    char recvbuf[0x1000];
    const int timeout_ms = 1000; // 1sec

    do {
        if ((conn = wintcp.tcpConnect(server_addr, server_port, false)) == nullptr) {
            dprintf("[run_echoclient_nonblockio] wintcp.tcp_connect(%s,%s) failed", server_addr.c_str(), server_port.c_str());
            break;
        }
        printf("Connected to %s:%d\n", conn->getIP().c_str(), conn->getPort());
        while (true) {
            printf(">>");
            getline(cin, line);
            linelen = line.size();
            if (!conn->sendAllNonblock(sizeof(int), (char*)&linelen, sizeof(int), timeout_ms, sendcnt)) {
                dprintf("send linelen failed");
                break;
            }
            if (!conn->sendAllNonblock(line.size(), (char*)line.c_str(), line.size(), timeout_ms, sendcnt)) {
                dprintf("send content failed");
                break;
            }
            if (!conn->recvAllNonblock(sizeof(int), (char*)&recvlen, sizeof(int), timeout_ms, recvcnt)) {
                dprintf("recv recvlen failed");
                break;
            }
            if (!conn->recvAllNonblock(recvlen, recvbuf, sizeof(recvbuf), timeout_ms, recvcnt)) {
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