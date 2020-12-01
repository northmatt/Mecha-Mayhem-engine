#pragma once
#include "Rendering.h"
#include "BLM.h"
#include "Time.h"
#include "Input.h"
#include <SoundSystem.h>

class Scene
{
public:
	Scene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f), bool physics = true);
	~Scene();

	entt::registry* GetRegistry();
	std::string GetName();

	virtual void Init(int windowWidth, int windowHeight);

	virtual Scene* Reattach();

	virtual void Update() {}
	virtual void Exit() { SoundManager::stopEverything(); }
	void BackEndUpdate();

protected:
	std::string m_name;
	short m_camCount = 1;

	entt::registry m_reg;
	btDiscreteDynamicsWorld *m_world = nullptr;
	HitboxGen m_colliders;
	Effects m_effects;

	btDbvtBroadphase *_broadphase = new btDbvtBroadphase();
	btCollisionConfiguration *_collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *_dispatcher = new btCollisionDispatcher(_collisionConfiguration);
	btSequentialImpulseConstraintSolver *_solver = new btSequentialImpulseConstraintSolver();
};

