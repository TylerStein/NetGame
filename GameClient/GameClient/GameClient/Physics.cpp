#include "Physics.h"

static Physics* m_instance;

Physics::Physics()
{
	m_physicsObjects = new std::vector<PhysicsObject*>();
}


Physics::~Physics()
{
	if (m_physicsObjects != nullptr) {
		m_physicsObjects->clear();
		delete m_physicsObjects;
		m_physicsObjects = nullptr;
	}
}

Physics * Physics::GetInstance()
{
	if (m_instance == nullptr) {
		m_instance = new Physics();
	}
	return m_instance;
}

void Physics::Shutdown()
{
	if (m_instance != nullptr) {
		delete m_instance;
		m_instance = nullptr;
	}
}

bool Physics::AddPhysicsObject(PhysicsObject* physicsObject)
{
	if (HasPhysicsObject(physicsObject)) return false;
	m_physicsObjects->push_back(physicsObject);
	return true;
}

bool Physics::RemovePhysicsObject(PhysicsObject* physicsObject)
{
	return false;
}

bool Physics::HasPhysicsObject(PhysicsObject* physicsObject) {
	return false;
}

bool Physics::Update(float deltaTime)
{
	PhysicsProperties props;
	for (auto iter = m_physicsObjects->begin(); iter < m_physicsObjects->end(); iter++) {
		PhysicsObject* physicsObject = *iter;

		float aX = physicsObject->Force.X() / physicsObject->PhysProps.mass;
		float aY = physicsObject->Force.Y() / physicsObject->PhysProps.mass;

		physicsObject->Force.Set(0, 0);

		physicsObject->Velocity += Vector2(aX * deltaTime, aY * deltaTime);
		Vector2 modVelocity = physicsObject->Velocity * physicsObject->PhysProps.friction * 2 * deltaTime;

		physicsObject->Velocity -= modVelocity;

		Vector2 projected = physicsObject->Position + physicsObject->Velocity;

		// TODO: Check for projected collision

		if (projected.X() < m_bounds_min.X()) {
			if (m_wrap) projected.X(m_bounds_max.X());
			else projected.X(m_bounds_min.X());
		}

		if (projected.X() > m_bounds_max.X()) {
			if (m_wrap) projected.X(m_bounds_min.X());
			else projected.X(m_bounds_max.X());
		}

		if (projected.Y() < m_bounds_min.Y()) {
			if (m_wrap) projected.Y(m_bounds_max.Y());
			else projected.Y(m_bounds_min.Y());
		}

		if (projected.Y() > m_bounds_max.Y()) {
			if (m_wrap) projected.Y(m_bounds_min.Y());
			else projected.Y(m_bounds_max.Y());
		}

		physicsObject->Position.Set(projected);
	}
	return true;
}

void Physics::SetBounds(Vector2 min, Vector2 max, bool wrap)
{
	m_wrap = wrap;
	m_bounds_min = min;
	m_bounds_max = max;
}
