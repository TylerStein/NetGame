#define OLC_PGE_APPLICATION

#include <iostream>
#include <unordered_map>

#include "NetClient.h"

#define _WINSOCK2API_
#define _WINSOCKAPI_

#include "olcPixelGameEngine.h"
#include "FormatUtil.h"
#include <sstream>

#include "GameObject.h"
#include "Physics.h"
#include "PhysicsObject.h"
#include "Vector2.h"

struct NetPlayerState
{
	bool active = false;
	GameObject* gameObject = nullptr;
};

class Game : public olc::PixelGameEngine
{
public:
	Game() {
		sAppName = "Game";
		m_net_players = std::unordered_map<std::string, NetPlayerState>();
		m_player = new PhysicsObject("Player");
		m_physics = Physics::GetInstance();

		m_physics->AddPhysicsObject(m_player);
	}

	~Game()	{
		//
	}

	void SetNetClient(NETCLIENT::NetClient* client) {
		if (hasNetClient && netClient != nullptr) {
			netClient->StopReceive();
		}

		netClient = client;
		hasNetClient = true;
		netClient->StartReceive();
		m_player->SetName(client->GetClientID());
	}


protected:
	bool hasNetClient;
	NETCLIENT::NetClient* netClient;
	Physics* m_physics;
	PhysicsObject* m_player;
	std::unordered_map<std::string, NetPlayerState> m_net_players;
	std::unordered_map<std::string, NetPlayerState>::iterator m_net_players_iter;
	Vector2 m_last_pos;
	bool m_init_net;
	float m_server_delay = 0.033f;
	float m_server_tick = 0.0f;

	// float m_mode_switch_tick = 0.0f;
	// float m_mode_switch_delay = 0.033f;
	// bool m_recv_mode = true;

	bool OnUserCreate() override {
		std::cout << "Created Game User" << std::endl;

		int32_t screenWidth = ScreenWidth();
		int32_t screenHeight = ScreenHeight();

		m_player->Position.Set(screenWidth / 2, screenHeight / 2);
		m_player->PhysProps.friction = 0.995f;
		
		m_last_pos = m_player->Position;

		m_physics->SetBounds(Vector2(0, 0), Vector2(screenWidth, screenHeight), true);

		// netClient->SetRecvMode(m_recv_mode);
		m_init_net = false;
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		int width = (int)ScreenWidth();
		int height = (int)ScreenHeight();

		bool allowUpdate = false;
		m_server_tick += fElapsedTime;
		if (m_server_tick >= m_server_delay) {
			m_server_tick = 0;
			allowUpdate = true;
		}

		// m_mode_switch_tick += fElapsedTime;
		// if (m_mode_switch_tick >= m_mode_switch_delay) {
		// 	m_recv_mode = !m_recv_mode;
		// 	netClient->SetRecvMode(m_recv_mode);
		// }

		Clear(olc::BLACK);
		DrawString(2, 14, std::to_string(fElapsedTime), olc::WHITE, 1U);
		DrawString(2, 26, std::to_string(m_server_tick) + " / " + std::to_string(m_server_delay), olc::WHITE, 1U);

		olc::HWButton key_up = GetKey(olc::W);
		olc::HWButton key_left = GetKey(olc::A);
		olc::HWButton key_right = GetKey(olc::D);
		olc::HWButton key_down = GetKey(olc::S);

		olc::HWButton key_r = GetKey(olc::R);

		const float moveSpeed = 0.74f;
		Vector2 force = Vector2();
		if (key_up.bPressed || key_up.bHeld) {
			force.Y(-moveSpeed);
		}
		else if (key_down.bPressed || key_down.bHeld) {
			force.Y(moveSpeed);
		}

		if (key_right.bPressed || key_right.bHeld) {
			force.X(moveSpeed);
		}
		else if (key_left.bPressed || key_left.bHeld) {
			force.X(-moveSpeed);
		}

		if (key_r.bPressed) {
			m_player->IterateColor();
		}

		if (GetKey(olc::Q).bPressed) {
			return false;
		}

		if (GetKey(olc::P).bPressed) {
			std::cout << "==Player List==" << std::endl;
			for (m_net_players_iter = m_net_players.begin(); m_net_players_iter != m_net_players.end(); m_net_players_iter++) {
				const char* active = m_net_players_iter->second.active ? "[ACTIVE] ": "[INACTIVE] ";
				std::cout << active << m_net_players_iter->first << " - " << m_net_players_iter->second.gameObject->GetName() << std::endl;
			}
		}

		Vector2 preRoundedPos = Vector2(std::roundf(m_player->Position.X()), std::roundf(m_player->Position.Y()));

		m_player->AddForce(force);
		m_physics->Update(fElapsedTime);
		m_player->Render(*this, true);

		Vector2 postRoundedPos = Vector2(std::roundf(m_player->Position.X()), std::roundf(m_player->Position.Y()));


		if (hasNetClient) {
			if (!m_init_net) {
				m_last_pos = postRoundedPos;
				m_init_net = true;
				netClient->SendPoint(m_last_pos.X(), m_last_pos.Y(), m_player->GetName());
			}

			DrawString(2, 4, "Connected as " + m_player->GetName(), olc::GREEN);
			std::queue<NETCLIENT::ClientMessage> messages = netClient->ConsumePendingMessages();

			if (allowUpdate) {
				if (m_last_pos.X() != postRoundedPos.X() || m_last_pos.Y() != postRoundedPos.Y()) {
					netClient->SendPoint(std::roundf(m_player->Position.X()), std::roundf(m_player->Position.Y()), m_player->GetName());
					m_last_pos = postRoundedPos;
				}
			}

			// if (!m_recv_mode) {
				while (!messages.empty()) {
					NETCLIENT::ClientMessage msg = messages.front();
					messages.pop();

					if (m_net_players[msg.client_id].active == false) {
						printf(std::string("Created client " + msg.client_id + " at (" + std::to_string(msg.client_x) + ", " + std::to_string(msg.client_y) + ")").c_str());
						std::cout << std::endl;
						m_net_players[msg.client_id].gameObject = new GameObject(msg.client_id);
						m_net_players[msg.client_id].active = true;
					}

					m_net_players[msg.client_id].gameObject->Position.Set(msg.client_x, msg.client_y);
				}
			// }


			m_net_players_iter = m_net_players.begin();
			DrawString(2, 64, "Other Players");
			int drawY = 70;

			for (m_net_players_iter = m_net_players.begin(); m_net_players_iter != m_net_players.end(); m_net_players_iter++) {
				m_net_players_iter->second.gameObject->Render(*this, true);
				DrawString(2, drawY, m_net_players_iter->second.gameObject->GetName(), olc::GREEN);
				drawY += 8;
			}

		}
		else {
			DrawString(2, 4, "Not Connected", olc::RED);
		}

		return true;
	}

	bool OnUserDestroy() override {
		std::cout << "Destroyed Game User" << std::endl;
		netClient->StopReceive();
		return true;
	}
};

bool TryConnect(char* id, char* ip, char* port) {
	return true;
}

int main()
{
	std::string client_id;
	std::string server_ip;
	std::string server_port;

	std::cout << "== Game Client ==" << std::endl;

	std::cout << "Enter Your Identifier: " << std::endl;
	std::getline(std::cin, client_id);

	// std::cout << "Enter the Server IP: " << std::endl;
	// std::getline(std::cin, server_ip);
	server_ip = "99.224.231.108";

	//std::cout << "Enter the Server Port: " << std::endl;
	//std::getline(std::cin, server_port);
	server_port = "63000";

	std::cout << "Trying connection to " << server_ip << ":" << server_port << " as " << client_id << std::endl;
	bool connected = true;
	if (!connected) {
		std::cout << "Failed to connect, press any key to exit" << std::endl;
		std::cin.ignore();
		return 0;
	}

	NETCLIENT::NetClient* netClient = new NETCLIENT::NetClient(client_id.c_str(), server_ip.c_str(), server_port.c_str());
	netClient->Initialize();
	Game game;
	if (game.Construct(720, 512, 1, 1)) {
		game.SetNetClient(netClient);
		game.Start();

	}

	//std::cout << "Ending Game, press any key to exit" << std::endl;
	//std::cin.ignore();

	delete netClient;
	netClient = nullptr;

	return 0;
}