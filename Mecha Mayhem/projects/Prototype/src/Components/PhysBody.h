#pragma once
#include "Components/Transform.h"

class PhysBody
{
public:
	PhysBody() {}
	~PhysBody() { m_body = nullptr; }

	//assign the world
	static void Init(btDiscreteDynamicsWorld *world);

	//unloads the world
	static void Unload();

	bool IsDynamic() { return m_dynamic; }
	bool Changed() { return m_changed; }

	//makes a box
	PhysBody& Init(float width, float depth, float height, glm::vec3 pos, float mass = 0, bool isDynamic = false);
	
	//makes a pill (currently doesn't work)
	PhysBody& Init(float radius, float height, glm::vec3 pos, float mass = 0, bool isDynamic = false);

	//makes a sphere
	PhysBody& Init(float radius, glm::vec3 pos, float mass = 0, bool isDynamic = false);

	btRigidBody* GetBody() { return m_body; }

	PhysBody& SetVelocity(glm::vec3 vel);
	PhysBody& SetVelocity(btVector3 vel);
	btVector3 GetVelocity() { return m_body->getLinearVelocity(); }

	PhysBody& SetPosition(glm::vec3 pos);
	PhysBody& SetPosition(btVector3 pos);


	PhysBody& SetRotation(glm::quat rot);
	PhysBody& SetRotation(btQuaternion rot);

	PhysBody& SetAwake(bool isAwake = true);

	//should only be used in Scene.cpp
	btTransform GetTransform();

private:
	btRigidBody* m_body = nullptr;
	bool m_dynamic = false;
	bool m_changed = true;

	static btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	static btDiscreteDynamicsWorld* m_world;
};

