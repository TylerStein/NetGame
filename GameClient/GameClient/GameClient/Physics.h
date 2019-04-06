#ifndef PHYSICS
#define PHYSICS

#include <vector>
#include "PhysicsObject.h"

class Physics
{
public:
	static Physics* GetInstance();
	static void Shutdown();

	bool AddPhysicsObject(PhysicsObject* physicsObject);
	bool RemovePhysicsObject(PhysicsObject* physicsObject);
	bool HasPhysicsObject(PhysicsObject* physicsObject);

	bool Update(float deltaTime);

	void SetBounds(Vector2 min, Vector2 max, bool wrap = false);
private:
	Physics();
	~Physics();
	std::vector<PhysicsObject*>* m_physicsObjects;

	bool m_wrap = false;
	Vector2 m_bounds_min = Vector2(-999, -999);
	Vector2 m_bounds_max = Vector2(999, 999);
};

#endif