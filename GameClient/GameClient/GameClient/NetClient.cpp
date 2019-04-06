#include "NetClient.h"

#include <WS2tcpip.h>
#include <queue>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include "FormatUtil.h"

#pragma comment(lib, "Ws2_32.lib")

namespace NETCLIENT {
	DWORD WINAPI receive(LPVOID lpParameter) {
		AsyncHook& hook = *((AsyncHook*)lpParameter);

		int result;
		int recvbuflen = DEFAULT_BUFLEN;
		char recvbuf[DEFAULT_BUFLEN];

		std::queue<std::string> backfill = std::queue<std::string>();
		// std::string last = std::string();

		bool hasLast = false;

		do {
			result = recv(hook.connect_socket, recvbuf, recvbuflen, 0);
			if (result > 0) {
				std::string readable = std::string(recvbuf).substr(0, result);
				std::string msg = std::string();

				for (int offset = 0; offset < result; offset += 32) {
					msg = readable.substr(offset, 32);
					if (backfill.size() > 1024) backfill.pop();
					backfill.push(msg);
				}


				if (!hook.pending_messages_busy) {
					hook.pending_messages_busy = true;
					while (!backfill.empty()) {
						hook.pending_messages.push(backfill.front());
						backfill.pop();
					}
					hook.pending_messages_busy = false;
				}
			}
			else if (result == 0) {
				std::cout << "Connection closed" << std::endl;
			}
			else {
				std::cout << "Recv failed " << WSAGetLastError() << std::endl;
			}

			memset(recvbuf, 0, sizeof recvbuf);

		} while (result > 0 && hook.keep_open);
		hook.keep_open = false;

		return 0;
	}

	NetClient::NetClient(const char* client_id, const char* server_ip, const char* server_port) {
		m_async_message_handler = AsyncHook();
		m_async_message_handler.keep_open = false;
		m_async_message_handler.pending_messages = std::queue<std::string>();
		m_async_message_handler.pending_messages_busy = false;

		m_server_ip = server_ip;
		m_client_id = client_id;
		m_server_port = server_port;
	}

	NetClient::~NetClient() {

	}

	void NetClient::Initialize() {
		WSADATA wsaData;
		int result;

		result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0) throw new std::exception("WSA Startup Failed: " + result);

		ZeroMemory(&addr_hints, sizeof(addr_hints));
		addr_hints.ai_family = AF_UNSPEC;
		addr_hints.ai_socktype = SOCK_STREAM;
		addr_hints.ai_protocol = IPPROTO_TCP;

		result = getaddrinfo(m_server_ip, m_server_port, &addr_hints, &addr_result);
		if (result != 0) {
			WSACleanup();
			throw new std::exception("WSA getaddrinfo failed: " + result);
		}

		addr_ptr = addr_result;
		m_connect_socket = socket(addr_ptr->ai_family, addr_ptr->ai_socktype, addr_ptr->ai_protocol);

		if (m_connect_socket == INVALID_SOCKET) {
			freeaddrinfo(addr_result);
			WSACleanup();
			std::string reason = std::string("Error at Socket(): " + WSAGetLastError());
			throw new std::exception(reason.c_str());
		}

		result = connect(m_connect_socket, addr_ptr->ai_addr, (int)addr_ptr->ai_addrlen);
		if (result == SOCKET_ERROR) {
			closesocket(m_connect_socket);
			m_connect_socket = INVALID_SOCKET;
		}

		freeaddrinfo(addr_result);

		if (m_connect_socket == INVALID_SOCKET) {
			WSACleanup();
			throw new std::exception("Unable to connect to server!");
		}

		m_async_message_handler.connect_socket = m_connect_socket;
	}

	void NetClient::StartReceive() {
		if (m_async_message_handler.keep_open == true) return;
		if (m_recv_handle != NULL) CloseHandle(m_recv_handle);

		m_async_message_handler.keep_open = true;
		m_recv_handle = CreateThread(0, 0, receive, &m_async_message_handler, 0, &m_recv_id);
	}

	void NetClient::StopReceive() {
		if (m_async_message_handler.keep_open == false) return;
		m_async_message_handler.keep_open = false;
	}

	void NetClient::SendPoint(int32_t x, int32_t y, std::string id) {
		std::string x_hex = "";
		std::string y_hex = "";
		std::string id_hex = ""
			;
		FormatUtil::Int32ToHexBE(x, x_hex);
		FormatUtil::Int32ToHexBE(y, y_hex);
		FormatUtil::TextToHex(id, id_hex);

		std::string out_hex = x_hex + y_hex + id_hex;
		const char* buff = out_hex.c_str();

		if (m_connect_socket != INVALID_SOCKET) {
			send(m_connect_socket, buff, out_hex.length(), 0);
		}
	}

	void NetClient::SetRecvMode(bool mode)
	{
		// m_async_message_handler.receive_mode = mode;
	}

	ClientMessage NetClient::RawToClientMessage(std::string hex) {
		std::string str_x_hex = hex.substr(0, 8);
		std::string str_y_hex = hex.substr(8, 8);
		std::string str_n_hex = hex.substr(16, 16);

		int32_t x = (int32_t)std::strtol(str_x_hex.c_str(), 0, 16);
		// FormatUtil::HexToInt32BE(str_x_hex, x);

		int32_t y = (int32_t)std::strtol(str_y_hex.c_str(), 0, 16);;
		// FormatUtil::HexToInt32BE(str_y_hex, y);

		std::string name;
		FormatUtil::HexToTextAlt(str_n_hex, name);

		ClientMessage res = ClientMessage();
		res.client_x = x;
		res.client_y = y;
		res.client_id = name;

		return res;
	}

	std::queue<ClientMessage> NetClient::ConsumePendingMessages() {
		std::queue<ClientMessage> clientMessages = std::queue<ClientMessage>();
		if (!m_async_message_handler.pending_messages_busy) {
			m_async_message_handler.pending_messages_busy = true;
			while (!m_async_message_handler.pending_messages.empty()) {
				ClientMessage clientMessage = RawToClientMessage(m_async_message_handler.pending_messages.front());
				clientMessages.push(clientMessage);
				m_async_message_handler.pending_messages.pop();
			}
			m_async_message_handler.pending_messages_busy = false;
		}
		return clientMessages;
	}
	std::string NetClient::GetClientID()
	{
		return std::string(m_client_id);
	}
}