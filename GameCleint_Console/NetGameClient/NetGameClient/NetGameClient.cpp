// NetGameClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <istream>
#include <string>
#include "Client.h"
#include "Display.h"

int main()
{
	std::cout << "Enter a client name (Max 12 Characters):" << std::endl;
	std::string client_id;
	std::getline(std::cin, client_id);
	if (client_id.length() > 12) client_id.resize(12);

	std::cout << "Please enter the server IP:" << std::endl;
	char* server_ip = new char[32];
	std::cin >> server_ip;
	std::cout << std::endl;

	const char* server_port = "63000";
	//std::cout << "Please enter the server port: ";
	//char* server_port = new char[8]
	//std::cin >> server_port;
	std::cout << std::endl;

	std::string serverDisplayName = std::string(server_ip) + ":" + std::string(server_port);
	std::cout << "Now attemtping to connect to " << serverDisplayName << " as " << client_id << std::endl;

	Client client = Client(client_id.c_str(), server_ip, server_port);
	std::string msg = "CONN:" + std::string(client_id);
	client.BeginReceive();
	client.Send(msg.c_str());

	std::string servermsg;
	std::string clientstring;
	
	std::vector<std::string> messages = std::vector<std::string>();
	std::cout << "Connected to server!" << std::endl;

	bool keepAlive = true;
	do {
		std::string clientstring;
		client.Receive();
		if (client.HasMessage()) {
			do {
				servermsg = std::string(client.PopMessage());
				messages.push_back(servermsg);
			} while (client.HasMessage());
		}
		std::cout << "== MESSAGES ==" << std::endl;
		for (int i = 0; i < messages.size(); i++) {
			std::cout << messages[i].c_str() << std::endl;
		}
		std::cout << "==============" << std::endl;

		std::cout << "== COMMANDS ==" << std::endl;
		std::cout << "Q = Quit" << std::endl;
		std::cout << "C = Check for new messages" << std::endl;
		std::cout << "L = List connected clients" << std::endl;
		std::cout << "Anything else to send a message to other clients (Limit 500 Characters)" << std::endl;
		std::cout << "==============" << std::endl;
		std::getline(std::cin, clientstring);

		if (clientstring.length() > 500) clientstring.resize(500);

		if (clientstring == "Q" || clientstring == "q") {
			keepAlive = false;
			std::cout << "Quitting message loop" << std::endl;
		} else if (clientstring == "C" || clientstring == "c") {
			system("cls");
			continue;
		} else if (clientstring == "L" || clientstring == "l") {
			clientstring = "LIST:CLIENTS";
			client.Send(clientstring.c_str());
		} else if (clientstring.length() > 0) {
			clientstring = "DATA:" + clientstring;
			client.Send(clientstring.c_str());
		}
		system("cls");
	} while (keepAlive);

	msg = "KILL:" + client_id;
	std::cout << "Disconnecting from " << server_ip << std::endl;
	client.Send(msg.c_str());
	
	client.EndReceive();
	client.Shutdown();

	server_ip = nullptr;
	server_port = nullptr;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
