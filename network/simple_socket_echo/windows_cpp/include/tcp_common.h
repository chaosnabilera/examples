#include <Windows.h>
#include <string>

bool WSAInitialize();
bool WSADeinitialize();
bool TCPConnect(const std::string& server_addr, const std::string& server_port, SOCKET* out_sock_conn);
bool TCPCreateListener(const std::string& server_port, SOCKET* out_sock_listen);
bool TCPAccept(SOCKET sock_listen, SOCKET* out_sock_client);