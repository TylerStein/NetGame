#include "PhysicsObject.h"


PhysicsObject::PhysicsObject() : GameObject()
{

}

PhysicsObject::PhysicsObject(std::string name) : GameObject(name)
{

}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::AddForce(Vector2 force) {
	Force += force;
}