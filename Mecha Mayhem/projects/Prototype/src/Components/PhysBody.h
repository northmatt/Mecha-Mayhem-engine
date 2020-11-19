#pragma once
#include "Components/Transform.h"

typedef btCollisionWorld::ClosestRayResultCallback RayResult;

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
	
	//makes a pill
	PhysBody& Init(float radius, float height, glm::vec3 pos, float mass = 0, bool isDynamic = false);

	//makes a sphere
	PhysBody& Init(float radius, glm::vec3 pos, float mass = 0, bool isDynamic = false);

	btRigidBody* GetBody() { return m_body; }

	PhysBody& SetGravity(glm::vec3 grav);
	PhysBody& SetGravity(btVector3 grav);

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

	static RayResult GetRaycastResult(btVector3 from, btVector3 to);
	static btVector3 GetRaycastWithDistanceLimit(glm::vec3 startPos, glm::vec3 look, float limit);
	static btVector3 GetRaycast(glm::vec3 startPos, glm::vec3 look);
	btVector3 GetRaycast(glm::vec3 look);

private:
	btRigidBody* m_body = nullptr;
	bool m_dynamic = false;
	bool m_changed = true;

	static btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	static btDiscreteDynamicsWorld* m_world;
};

