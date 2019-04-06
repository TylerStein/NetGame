#ifndef NETWORK_AGENT
#define NETWORK_AGENT

#include "GameObject.h"

class NetworkAgent
{
public:
	NetworkAgent();
	NetworkAgent(GameObject* gameObject);
	~NetworkAgent();

	void SetGameObject(GameObject* gameObject);
	void ClearGameObject();

private:
	GameObject* m_gameObject;
};
#endif