#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

WinSock::WinSock(SOCKET _sock, sockaddr* _addr, bool _is_blocking) {
    char ipstr[INET6_ADDRSTRLEN] = { 0 };
    memset(addr, 0, sizeof(addr));
    sock = _sock;
    is_blocking = _is_blocking;

    if (sock == INVALID_SOCKET)
        return;
    if (!switchBlockingMode(is_blocking, true))
        close();
    
    int sin_family = ((struct sockaddr_in*)_addr)->sin_family;
    if (sin_family == AF_INET) {
        struct sockaddr_in* sin = (struct sockaddr_in*)addr;
        memcpy(addr, _addr, sizeof(sockaddr_in));
        inet_ntop(AF_INET, &(sin->sin_addr), ipstr, sizeof(ipstr));
        ip = ipstr;
        port = htons(sin->sin_port);
    }
    else if (sin_family == AF_INET6) {
        struct sockaddr_in6* sin6 = (struct sockaddr_in6*)addr;
        memcpy(addr, _addr, sizeof(sockaddr_in6));
        inet_ntop(AF_INET6, &(sin6->sin6_addr), ipstr, sizeof(ipstr));
        ip = ipstr;
        port = htons(sin6->sin6_port);
    }
    else {
        dprintf("Unknown sin_family:%d", sin_family);
    }
}

WinSock::~WinSock() {
    close();
}

bool WinSock::recvAllBlock(size_t recvlen, char* buf, size_t buflen, size_t& recvcnt) {
    const size_t max_readsize_per_recv = 0x10000000;
    bool success = false;
    int recvres = -1;

    do {
        if (buflen < recvlen) {
            dprintf("buflen(%llu) smaller than recvlen");
            break;
        }
        if (!switchBlockingMode(true)) {
            dprintf("Failed to change socket to blocking mode");
            break;
        }

        recvcnt = 0;
        while (recvcnt < recvlen) {
            int cur_recv_size = min(max_readsize_per_recv, recvlen - recvcnt);
            recvres = recv(sock, buf + recvcnt, cur_recv_size, MSG_WAITALL);
            if (recvres == 0) {
                dprintf("graceful shutdown during recv_all_block");
                break;
            }
            else if (recvres == SOCKET_ERROR) {
                dprintf("recv error: %d", GetLastError());
                break;
            }
            recvcnt += recvres;
        }
        success = (recvcnt == recvlen);
    } while (0);

    return success;
}

bool WinSock::sendAllBlock(size_t sendlen, char* buf, size_t buflen, size_t& sendcnt) {
    const size_t max_sendsize_per_recv = 0x10000000;
    bool success = false;
    int sendres = -1;

    do {
        if (buflen < sendlen) {
            dprintf("buflen(%llu) smaller than recvlen");
            break;
        }
        if (!switchBlockingMode(true)) {
            dprintf("Failed to change socket to blocking mode");
            break;
        }

        sendcnt = 0;
        while (sendcnt < sendlen) {
            int cur_send_size = min(max_sendsize_per_recv, sendlen - sendcnt);
            sendres = send(sock, buf + sendcnt, cur_send_size, 0);
            if (sendres == SOCKET_ERROR) {
                dprintf("recv error: %d", GetLastError());
                break;
            }
            sendcnt += sendres;
        }
        success = (sendcnt == sendlen);
    } while (0);

    return success;
}

bool WinSock::recvAllNonblock(size_t recvlen, char* buf, size_t buflen, int timeout_ms, size_t& recvcnt) {
    const size_t max_readsize_per_recv = 0x10000000;
    const int ms_per_sleep = (timeout_ms < 50) ? 1 : 10;
    bool success = false;
    int recvres = -1;
    DWORD err = -1;
    ULONGLONG tbeg;

    do {
        if (buflen < recvlen) {
            dprintf("buflen(%llu) smaller than recvlen");
            break;
        }
        if (!switchBlockingMode(false)) {
            dprintf("Failed to change socket to non-blocking mode");
            break;
        }
        recvcnt = 0;
        tbeg = GetTickCount64();
        while (recvcnt < recvlen && GetTickCount64()-tbeg < timeout_ms) {
            int cur_recv_size = min(max_readsize_per_recv, recvlen - recvcnt);
            recvres = recv(sock, buf + recvcnt, cur_recv_size, 0);
            if (recvres == 0) {
                dprintf("graceful shutdown during recv_all_block");
                break;
            }
            else if (recvres == SOCKET_ERROR) {
                if ((err = GetLastError()) == WSAEWOULDBLOCK) {
                    Sleep(ms_per_sleep);
                }
                else {
                    dprintf("recv error: %d", err);
                    break;
                }
            }
            else {
                recvcnt += recvres;
            }
        }
        if (GetTickCount64() - tbeg >= timeout_ms) {
            dprintf("[recv_all_nonblock] Timeout occurred!");
        }
        success = (recvcnt == recvlen);
    } while (0);

    return success;
}

bool WinSock::sendAllNonblock(size_t sendlen, char* buf, size_t buflen, int timeout_ms, size_t& sendcnt) {
    const size_t max_sendsize_per_recv = 0x10000000;
    const int ms_per_sleep = (timeout_ms < 50) ? 1 : 10;
    bool success = false;
    int sendres = -1;
    DWORD err = -1;
    ULONGLONG tbeg;

    do {
        if (buflen < sendlen) {
            dprintf("buflen(%llu) smaller than recvlen");
            break;
        }
        if (!switchBlockingMode(false)) {
            dprintf("Failed to change socket to non-blocking mode");
            break;
        }
        sendcnt = 0;
        tbeg = GetTickCount64();
        while (sendcnt < sendlen && GetTickCount64() - tbeg < timeout_ms) {
            int cur_send_size = min(max_sendsize_per_recv, sendlen - sendcnt);
            sendres = send(sock, buf + sendcnt, cur_send_size, 0);
            if (sendres == SOCKET_ERROR) {
                if ((err = GetLastError()) == WSAEWOULDBLOCK) {
                    Sleep(ms_per_sleep);
                }
                else {
                    dprintf("send error: %d", err);
                    break;
                }
            }
            else {
                sendcnt += sendres;
            }
        }
        if (GetTickCount64() - tbeg >= timeout_ms) {
            dprintf("[send_all_nonblock] Timeout occurred!");
        }
        success = (sendcnt == sendlen);
    } while (0);

    return success;
}

string WinSock::getIP() {
    return ip;
}

uint16_t WinSock::getPort() {
    return port;
}