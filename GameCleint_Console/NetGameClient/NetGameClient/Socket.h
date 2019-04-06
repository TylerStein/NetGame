#ifndef CONN_SOCKET
#define CONN_SOCKET

#include <string>
#include <stdlib.h>
#include <winsock2.h>

class Socket
{
public:
	Socket(char* socket);
	~Socket();

	bool Initialize();

private:
	char* m_socket;
	addrinfo* m_addrinfo;
};

#endif