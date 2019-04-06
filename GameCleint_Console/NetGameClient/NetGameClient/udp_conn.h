#ifndef UDP_CONN;
#define UDP_CONN;

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

namespace udp_conn {
	class udp_client {
	public:
		int get_socket() const;
		int get_port() const;
		std::string get_ip() const;
		std::string get_client_id() const;
		int send(const char *data, size_t size);

	private:
		int m_socket;
		int m_port;
		std::string m_addr;
		std::string* m_client_id;
		struct addrinfo* m_addrinfo;
	};
}

#endif