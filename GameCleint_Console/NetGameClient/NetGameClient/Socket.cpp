#include "pch.h"
#include "Socket.h"
#include <WinSock2.h>
#include <>

Socket::Socket(char* socket)
{
	m_socket = socket;
}


Socket::~Socket()
{
	delete m_socket;
}

bool Socket::Initialize()
{
	WSAData wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return false;
	}
}
