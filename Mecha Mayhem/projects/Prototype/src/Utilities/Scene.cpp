#include "Scene.h"

Scene::Scene(const std::string& name, const glm::vec3& gravity, bool physics)
	: m_name(name)
{
	if (physics) {
		m_world = new btDiscreteDynamicsWorld(
			_dispatcher, _broadphase, _solver, _collisionConfiguration);

		m_world->setGravity(BLM::GLMtoBT(gravity));
	}
}

Scene::~Scene()
{
	Rendering::hitboxes = nullptr;
	Rendering::effects = nullptr;
	Rendering::frameEffects = nullptr;
	if (m_world != nullptr)
	delete m_world;
}

entt::registry* Scene::GetRegistry()
{
	return &m_reg;
}

std::string Scene::GetName()
{
	return m_name;
}

void Scene::Init(int windowWidth, int windowHeight)
{
	//generic init, use as guidelines
	ECS::AttachRegistry(&m_reg);
	if (m_world) {
		PhysBody::Init(m_world);
		ECS::AttachWorld(m_world);
		if (!m_colliders.Init(m_world, "maps/map1"))
			std::cout << "map1 failed to load, no collision boxes loaded\n";
	}
	m_frameEffects.Init(windowWidth, windowHeight);

	// for multi cam setup, change the m_camCount variable, and also spawn in reverse order (aka player 1 last)
	auto cameraEnt = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
	camCam.SetFovDegrees(60.f).ResizeWindow(windowWidth, windowHeight);
}

Scene* Scene::Reattach()
{
	ECS::AttachRegistry(&m_reg);
	if (m_world) {
		PhysBody::Init(m_world);
		ECS::AttachWorld(m_world);
		Rendering::hitboxes = &m_colliders;
	}

	SoundManager::stopEverything();

	m_frameEffects.Resize(BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);

	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;

	return this;
}

void Scene::BackEndUpdate()
{
	if (!m_paused) {
		if (m_world != nullptr) {

			m_world->stepSimulation(Time::dt, 10);

			m_reg.view<PhysBody, Transform>().each(
				[](PhysBody& phys, Transform& trans) {
					if (phys.IsDynamic() || phys.Changed())
						trans.SetTransform(phys.GetTransform());
				}
			);
		}
		m_effects.Update();
	}


	//always render
	if (m_camCount < 1)
		m_camCount = 1;
	else if (m_camCount > 4)
		m_camCount = 4;

	Rendering::Update(&m_reg, m_camCount, m_paused);

	if (m_paused)   if (m_pauseSprite.IsValid()) {
		Rendering::DrawPauseScreen(m_pauseSprite);
	}

	m_frameEffects.UnBind();
	m_frameEffects.Draw();
}

int Scene::ChangeScene(int sceneCount)
{
	if (m_nextScene >= sceneCount)
		return -1;

	int nextScene = m_nextScene;
	m_nextScene = -1;

	return nextScene;
}
