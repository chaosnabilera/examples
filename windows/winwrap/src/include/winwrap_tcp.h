#pragma once

#include <Windows.h>

#include <memory>
#include <mutex>
#include <string>

class WinSock;
class WinAcceptSock;

class WinTCP {
public:
	~WinTCP();
	WinTCP& get_instance();

	// return connected WinSock on success. return nullptr on connection failure
	WinSock* tcp_connect(std::string server_addr, std::string service_port, bool blocking);

	// return ready-to-accept WinAcceptSock on success. return nullptr on socket/bind/listen failure
	WinAcceptSock* tcp_create_listener(std::string server_addr, std::string service_port, bool blocking);
private:
	static std::unique_ptr<WinTCP> instance;
	static std::once_flag m_onceflag;
	WinTCP();
	bool wsa_startup_success;
};

class WinAcceptSock {
public:
	friend class WinTCP;
	~WinAcceptSock();
	WinSock* accept_block();             // block till accept occurs. Return nullptr on error
	bool accept_nonblock(WinSock*& res); // return true on accept/EWOULDBLOCK. return false on error
	void close();
private:
	WinAcceptSock(SOCKET sock, bool is_blocking);
	SOCKET sock;
	bool is_blocking;
};

class WinSock {
public:
	friend class WinTCP;
	friend class WinAcceptSock;
	~WinSock();
	void close();
	bool can_read();
	bool can_write();
	bool recv_all_block(UINT64 recvlen, char* buf, UINT64 buflen, UINT64& recvcnt);
	bool send_all_block(UINT64 sendlen, char* buf, UINT64 buflen, UINT64& sendcnt);
	bool recv_all_nonblock(UINT64 recvlen, char* buf, UINT64 buflen, int timeout_ms, UINT64& recvcnt);
	bool send_all_nonblock(UINT64 sendlen, char* buf, UINT64 buflen, int timeout_ms, UINT64& sendcnt);
private:
	WinSock(SOCKET sock, sockaddr addr, bool is_blocking);
	SOCKET sock;
	bool is_blocking;
	sockaddr addr;
};