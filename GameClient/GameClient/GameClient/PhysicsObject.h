#ifndef PHYSICS_OBJECT
#define PHYSICS_OBJECT

#include "GameObject.h"

struct PhysicsProperties
{
	float mass = 1.0f;
	float friction = 0.98f;
};

class PhysicsObject : public GameObject
{
	friend class Physics;
public:
	Vector2 Velocity = Vector2();
	Vector2 Force = Vector2();
	PhysicsProperties PhysProps;

	PhysicsObject();
	PhysicsObject(std::string name);

	~PhysicsObject();

	void AddForce(Vector2 force);
protected:
};

#endif