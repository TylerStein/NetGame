#include "pch.h"
#include "Client.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

Client::Client(const char* client_id, const char* server_ip, const char* port)
{
	m_client_id = client_id;
	m_server_ip = server_ip;
	m_port = port;
	m_message_queue = new std::queue<std::string>();
	m_async_message_handler.keep_open = false;
	m_async_message_handler.pending_queue = std::queue<std::string>();
	m_async_message_handler.pending_queue_busy = false;

	WSADATA wsaData;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0) {
		std::string reason = std::string("WSA Startup Failed: " + result);
		throw new ClientException(reason.c_str(), m_client_id, m_server_ip, m_port);
	}

	ZeroMemory(&m_hints, sizeof(m_hints));
	m_hints.ai_family = AF_UNSPEC;
	m_hints.ai_socktype = SOCK_STREAM;
	m_hints.ai_protocol = IPPROTO_TCP;

	result = getaddrinfo(m_server_ip, m_port, &m_hints, &m_result);
	if (result != 0) {
		WSACleanup();
		std::string reason = std::string("GetAddrInfo Failed: " + result);
		throw new ClientException(reason.c_str(), m_client_id, m_server_ip, m_port);
	}

	m_addr_ptr = m_result;
	m_connect_socket = socket(m_addr_ptr->ai_family, m_addr_ptr->ai_socktype, m_addr_ptr->ai_protocol);

	if (m_connect_socket == INVALID_SOCKET) {
		freeaddrinfo(m_result);
		WSACleanup();
		std::string reason = std::string("Error at Socket(): " + WSAGetLastError());
		throw new ClientException(reason.c_str(), m_client_id, m_server_ip, m_port);
	}

	result = connect(m_connect_socket, m_addr_ptr->ai_addr, (int)m_addr_ptr->ai_addrlen);
	if (result == SOCKET_ERROR) {
		closesocket(m_connect_socket);
		m_connect_socket = INVALID_SOCKET;
	}

	freeaddrinfo(m_result);

	if (m_connect_socket == INVALID_SOCKET) {
		WSACleanup();
		throw new ClientException("Unable to connect to server!", m_client_id, m_server_ip, m_port);
	}

	m_async_message_handler.connect_socket = m_connect_socket;
}

Client::~Client()
{
	m_client_id = nullptr;
	m_server_ip = nullptr;
	delete m_message_queue;

	WSACleanup();

	m_port = nullptr;
	m_result = nullptr;
	m_addr_ptr = nullptr;
}

void Client::Send(const char* buffer) const {

	int result;
	result = send(m_connect_socket, buffer, (int)strlen(buffer), 0);
	if (result == SOCKET_ERROR) {
		closesocket(m_connect_socket);
		WSACleanup();
		throw new ClientException(std::string("Send Failed: " + WSAGetLastError()).c_str(), m_client_id, m_server_ip, m_port);
	}


}

void Client::Receive() {
	if (m_async_message_handler.keep_open) {
		if (m_async_message_handler.pending_queue_busy == false) {
			m_async_message_handler.pending_queue_busy = true;

			while (m_async_message_handler.pending_queue.empty() == false) {
				std::string last = m_async_message_handler.pending_queue.front();
				m_async_message_handler.pending_queue.pop();
				m_message_queue->push(last);
			}

			m_async_message_handler.pending_queue_busy = false;
		}
		else {
			printf("Receive() Pendique Queue Busy");
		}
	}
}


DWORD WINAPI receive_old(LPVOID lpParameter) {
	AsyncMessageHandler& handler = *((AsyncMessageHandler*)lpParameter);

	unsigned int counter = 0;
	do {
		if (handler.pending_queue_busy == false) {
			handler.pending_queue_busy = true;

			handler.pending_queue.push("HELLO NO." + std::to_string(counter));

			handler.pending_queue_busy = false;
			if (counter < 0xFFFFFFFF) counter++;
		}
		else {
			printf("async receive(): Pending queue busy, retrying in 100ms");
		}
		Sleep(100);
	} while (handler.keep_open);

	return 0;
}

DWORD WINAPI receive(LPVOID lpParameter) {
	AsyncMessageHandler& handler = *((AsyncMessageHandler*)lpParameter);

	int result;
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];

	std::queue<std::string> backfill = std::queue<std::string>();

	do {
		result = recv(handler.connect_socket, recvbuf, recvbuflen, 0);
		if (result > 0) {
			std::string readable = std::string(recvbuf).substr(0, result);

			if (handler.pending_queue_busy == false) {
				handler.pending_queue_busy = true;
				handler.pending_queue.push(readable);

				while (backfill.empty() == false) {
					handler.pending_queue.push(backfill.front());
					backfill.pop();
				}

				handler.pending_queue_busy = false;
			}
			else {
				backfill.push(readable);
			}
		}
		else if (result == 0) {
			printf("connection closed\n");
		}
		else {
			printf("recv failed %d\n", WSAGetLastError());
		}
	} while (result > 0 && handler.keep_open);

	handler.keep_open = false;

	return 0;
}

void Client::BeginReceive() {
	if (m_async_message_handler.keep_open == true) return;
	if (m_recv_handle != NULL) CloseHandle(m_recv_handle);
	m_async_message_handler.keep_open = true;
	m_recv_handle = CreateThread(0, 0, receive, &m_async_message_handler, 0, &m_recv_id);
}

void Client::EndReceive() {
	if (m_async_message_handler.keep_open == false) return;
	m_async_message_handler.keep_open = false;
}

void Client::Shutdown() const {
	int result = shutdown(m_connect_socket, SD_SEND);
	if (result == SOCKET_ERROR) {
		closesocket(m_connect_socket);
		WSACleanup();
		throw new ClientException(std::string("Shutdown failed: " + WSAGetLastError()).c_str(), m_client_id, m_server_ip, m_port);
	}
}

bool Client::HasMessage() const {
	return (m_message_queue->empty() == false);
}

std::string Client::PopMessage() const {
	if (HasMessage() == false) {
		throw new ClientException("No more messages in the queue", m_client_id, m_server_ip, m_port);
	}
	std::string message = m_message_queue->front();
	m_message_queue->pop();
	return message;
}