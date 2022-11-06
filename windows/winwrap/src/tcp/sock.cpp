#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "dprintf.hpp"
#include "winwrap_tcp.h"

using namespace std;

Sock::~Sock() {
}

void Sock::close() {
    if (sock != INVALID_SOCKET) {
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
}

bool Sock::canRead() {
    int sres;
    TIMEVAL immediate = { 0,0 };
    fd_set fds;

    if (sock == INVALID_SOCKET)
        return false;

    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    if ((sres = select(0, &fds, nullptr, nullptr, &immediate)) == SOCKET_ERROR) {
        dprintf("Error occurred during can_read select : %d",WSAGetLastError());
        close();
    }

    return (sres > 0);
}

bool Sock::canWrite() {
    int sres;
    TIMEVAL immediate = { 0,0 };
    fd_set fds;

    if (sock == INVALID_SOCKET)
        return false;

    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    if ((sres = select(0, nullptr, &fds, nullptr, &immediate)) == SOCKET_ERROR) {
        dprintf("Error occurred during can_write select : %d",WSAGetLastError());
        close();
    }

    return (sres > 0);
}

bool Sock::switchBlockingMode(bool should_block, bool force_setting) {
    if (sock == INVALID_SOCKET) {
        dprintf("[switch_blocking_mode] sock is invalid");
        return false;
    }

    if (force_setting || is_blocking != should_block) {
        u_long mode = should_block ? 0 : 1;
        if (ioctlsocket(sock, FIONBIO, &mode) != NO_ERROR) {
            dprintf("set_socket_blocking_mode(%p, %d) failed", sock, should_block);
            return false;
        }
        is_blocking = should_block;
    }
    return true;
}

