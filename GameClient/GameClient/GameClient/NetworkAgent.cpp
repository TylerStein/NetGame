#include "NetworkAgent.h"



NetworkAgent::NetworkAgent()
{
	m_gameObject = nullptr;
}

NetworkAgent::NetworkAgent(GameObject * gameObject)
{
	m_gameObject = gameObject;
}


NetworkAgent::~NetworkAgent()
{
	m_gameObject = nullptr;
}

void NetworkAgent::SetGameObject(GameObject * gameObject)
{
	m_gameObject = gameObject;
}

void NetworkAgent::ClearGameObject()
{
	m_gameObject = nullptr;
}
