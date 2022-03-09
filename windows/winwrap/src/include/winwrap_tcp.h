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
	static WinTCP& get_instance();
	
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

class Sock {
public:
	virtual ~Sock() = 0;
	void close();
	bool can_read();
	bool can_write();
protected:
	bool switch_blocking_mode(bool should_block, bool force_setting = false);
	SOCKET sock;
	bool is_blocking;
};

class WinAcceptSock : public Sock{
public:
	friend class WinTCP;
	~WinAcceptSock();
	WinSock* accept_block();             // block till accept occurs. Return nullptr on error
	bool accept_nonblock(WinSock*& res); // return true on accept/EWOULDBLOCK. return false on error
private:
	WinAcceptSock(SOCKET sock, bool is_blocking);
};

class WinSock : public Sock {
public:
	friend class WinTCP;
	friend class WinAcceptSock;
	~WinSock();
	bool recv_all_block(size_t recvlen, char* buf, size_t buflen, size_t& recvcnt);
	bool send_all_block(size_t sendlen, char* buf, size_t buflen, size_t& sendcnt);
	bool recv_all_nonblock(size_t recvlen, char* buf, size_t buflen, int timeout_ms, size_t& recvcnt);
	bool send_all_nonblock(size_t sendlen, char* buf, size_t buflen, int timeout_ms, size_t& sendcnt);

	std::string get_ip();
	uint16_t get_port();
private:
	WinSock(SOCKET sock, sockaddr* addr, bool is_blocking);
	char addr[0x20]; // sizeof(sockaddr_in6) = 28 < 32
	std::string ip;
	uint16_t port;
};