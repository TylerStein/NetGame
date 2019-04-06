#ifndef NET_CLIENT
#define NET_CLIENT

#include <WinSock2.h>
#include <queue>

#define DEFAULT_BUFLEN 512

namespace NETCLIENT {
	struct ClientMessage
	{
		std::string client_id;
		int client_x;
		int client_y;
	};

	struct AsyncHook
	{
		std::queue<std::string> pending_messages;
		SOCKET connect_socket;
		bool pending_messages_busy = false;
		bool keep_open = false;
	};


	class NetClient
	{
	public:
		NetClient(const char* client_id, const char* server_ip, const char* server_port);
		~NetClient();

		void Initialize();

		void StartReceive();
		void StopReceive();

		void SendPoint(int32_t x, int32_t y, std::string id);
		void SetRecvMode(bool mode);

		ClientMessage RawToClientMessage(std::string raw);
		std::queue<ClientMessage> ConsumePendingMessages();

		std::string GetClientID();

	private:
		AsyncHook m_async_message_handler;

		HANDLE m_recv_handle;
		DWORD m_recv_id;

		addrinfo addr_hints;
		addrinfo* addr_result = NULL;
		addrinfo* addr_ptr = NULL;

		const char* m_client_id;
		const char* m_server_ip;
		const char* m_server_port;

		SOCKET m_connect_socket = INVALID_SOCKET;
	};
}

#endif