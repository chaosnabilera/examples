#include <iostream>
#include <string>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

void run_echoserver_blockio(string server_addr, string server_port) {
    WinTCP& wintcp = WinTCP::getInstance();
    WinAcceptSock* as = nullptr;
    WinSock* s = nullptr;

    int recvlen;
    size_t recvcnt, sendcnt;
    char recvbuf[0x1000];

    do {
        if ((as = wintcp.tcpCreateListener(server_addr, server_port, true)) == nullptr) {
            dprintf("wintcp.tcp_create_listener(%s,%s) failed", server_addr.c_str(), server_port.c_str());
            break;
        }
        while ((s = as->acceptBlock()) != nullptr) {
            printf("Connection from %s:%d\n", s->getIP().c_str(), s->getPort());
            while (true) {
                if (!s->recvAllBlock(sizeof(int), (char*)&recvlen, sizeof(int), recvcnt)) {
                    dprintf("recv recvlen failed");
                    break;
                }
                if (!s->recvAllBlock(recvlen, recvbuf, sizeof(recvbuf), recvcnt)) {
                    dprintf("recv content failed");
                    break;
                }
                recvbuf[recvlen] = '\0';
                printf("recv(%d): %s\n", recvlen, recvbuf);
                if (!s->sendAllBlock(sizeof(int), (char*)&recvlen, sizeof(int), sendcnt)) {
                    dprintf("send recvlen failed");
                    break;
                }
                if (!s->sendAllBlock(recvlen, recvbuf, sizeof(recvbuf), sendcnt)) {
                    dprintf("send content failed");
                    break;
                }
            }
            delete s;
        }
    } while (0);
}

void run_echoserver_nonblockio(string server_addr, string server_port) {
    WinTCP& wintcp = WinTCP::getInstance();
    WinAcceptSock* as = nullptr;
    WinSock* s = nullptr;
    const int accept_poll_period_ms = 10;

    int recvlen;
    size_t recvcnt, sendcnt;
    char recvbuf[0x1000];
    int timeout_msgstart = 60*1000; // wait 1min for start of message
    int timeout_ms = 1000;

    do {
        if ((as = wintcp.tcpCreateListener(server_addr, server_port, false)) == nullptr) {
            dprintf("wintcp.tcp_create_listener(%s,%s) failed", server_addr.c_str(), server_port.c_str());
            break;
        }
        while (as->acceptNonblock(s)) {
            if (s == nullptr) {
                Sleep(accept_poll_period_ms);
                continue;
            }
            printf("Connection from %s:%d\n", s->getIP().c_str(), s->getPort());
            while (true) {
                if (!s->recvAllNonblock(sizeof(int), (char*)&recvlen, sizeof(int), timeout_msgstart, recvcnt)) {
                    dprintf("recv recvlen failed");
                    break;
                }
                if (!s->recvAllNonblock(recvlen, recvbuf, sizeof(recvbuf), timeout_ms, recvcnt)) {
                    dprintf("recv content failed");
                    break;
                }
                recvbuf[recvlen] = '\0';
                printf("recv(%d): %s\n", recvlen, recvbuf);
                if (!s->sendAllNonblock(sizeof(int), (char*)&recvlen, sizeof(int), timeout_ms, sendcnt)) {
                    dprintf("send recvlen failed");
                    break;
                }
                if (!s->sendAllNonblock(recvlen, recvbuf, sizeof(recvbuf), timeout_ms, sendcnt)) {
                    dprintf("send content failed");
                    break;
                }
            }
            delete s;
        }
    } while (0);
}