#ifndef CONN_CLIENT
#define CONN_CLIENT

#include <string>
#include <queue>
#include <exception>
#include <WinSock2.h>

#define DEFAULT_BUFLEN 512

struct AsyncMessageHandler
{
	std::queue<std::string> pending_queue;
	SOCKET connect_socket;
	bool pending_queue_busy = false;
	bool keep_open = false;
};

class ClientException : public std::exception
{
public:
	ClientException(const char* reason, const char* client_id, const char* server_ip, const char* port) 
	{
		m_reason = reason;
		m_port = port;
		m_client_id = client_id;
		m_server_ip = server_ip;
	}

	const char* Description()
	{
		std::string response = "(client=" + std::string(m_client_id) + ", server=" + std::string(m_server_ip) + ", port = " + std::string(m_port) + ") " + std::string(m_reason);
		return response.c_str();
	}
private:
	const char* m_port;
	const char* m_reason;
	const char* m_client_id;
	const char* m_server_ip;
};

class Client
{
public:
	Client(const char* client_id, const char* server_ip, const char* port);
	~Client();

	void Send(const char* buffer) const;
	void Receive();

	void BeginReceive();
	void EndReceive();

	void Shutdown() const;
	bool HasMessage() const;
	std::string PopMessage() const;


private:
	const uint16_t m_max_queued_messages = 256;

	std::queue<std::string>* m_message_queue;
	bool m_message_queue_busy;

	AsyncMessageHandler m_async_message_handler;

	HANDLE m_recv_handle;
	DWORD m_recv_id;

	const char* m_client_id;
	const char* m_server_ip;
	const char* m_port;

	struct addrinfo* m_result = NULL;
	struct addrinfo* m_addr_ptr = NULL;
	struct addrinfo m_hints;

	SOCKET m_connect_socket = INVALID_SOCKET;
};

#endif