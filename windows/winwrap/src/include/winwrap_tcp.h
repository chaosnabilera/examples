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
	static WinTCP& getInstance();
	
	// return connected WinSock on success. return nullptr on connection failure
	WinSock* tcpConnect(std::string server_addr, std::string service_port, bool blocking);

	// return ready-to-accept WinAcceptSock on success. return nullptr on socket/bind/listen failure
	WinAcceptSock* tcpCreateListener(std::string server_addr, std::string service_port, bool blocking);
private:
	static std::unique_ptr<WinTCP> instance;
	static std::once_flag mOnceFlag;
	WinTCP();
	bool wsaStartupSuccess;
};

class Sock {
public:
	virtual ~Sock() = 0;
	void close();
	bool canRead();
	bool canWrite();
protected:
	bool switchBlockingMode(bool should_block, bool force_setting = false);
	SOCKET sock;
	bool is_blocking;
};

class WinAcceptSock : public Sock{
public:
	friend class WinTCP;
	~WinAcceptSock();
	WinSock* acceptBlock();             // block till accept occurs. Return nullptr on error
	bool acceptNonblock(WinSock*& res); // return true on accept/EWOULDBLOCK. return false on error
private:
	WinAcceptSock(SOCKET sock, bool is_blocking);
};

class WinSock : public Sock {
public:
	friend class WinTCP;
	friend class WinAcceptSock;
	~WinSock();
	bool recvAllBlock(size_t recvlen, char* buf, size_t buflen, size_t& recvcnt);
	bool sendAllBlock(size_t sendlen, char* buf, size_t buflen, size_t& sendcnt);
	bool recvAllNonblock(size_t recvlen, char* buf, size_t buflen, int timeout_ms, size_t& recvcnt);
	bool sendAllNonblock(size_t sendlen, char* buf, size_t buflen, int timeout_ms, size_t& sendcnt);

	std::string getIP();
	uint16_t getPort();
private:
	WinSock(SOCKET sock, sockaddr* addr, bool is_blocking);
	char addr[0x20]; // sizeof(sockaddr_in6) = 28 < 32
	std::string ip;
	uint16_t port;
};