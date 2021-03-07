#include "Scene.h"

std::vector<Scene*> Scene::m_scenes;
Scene* Scene::m_activeScene = nullptr;
size_t Scene::m_nextScene = 0;
bool Scene::m_doSceneChange = false;
bool Scene::m_exitGame = false;

Scene::Scene(const std::string& name, const glm::vec3& gravity, bool physics)
	: m_name(name)
{
	if (physics) {
		_broadphase = new btDbvtBroadphase();
		_collisionConfiguration = new btDefaultCollisionConfiguration();
		_dispatcher = new btCollisionDispatcher(_collisionConfiguration);
		_solver = new btSequentialImpulseConstraintSolver();

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
	if (m_world != nullptr) {
		delete m_world;
	}
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
	else {
		PhysBody::Init(nullptr);
		ECS::AttachWorld(nullptr);
		Rendering::hitboxes = nullptr;
	}

	m_frameEffects.Resize(BackEnd::GetWidth(), BackEnd::GetHeight());

	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;

	return this;
}

void Scene::BackEndUpdate()
{
	if (!m_paused) {
		//only update these when paused, so put here
		Player::Update();

		//update components
		m_reg.view<ObjMorphLoader, Transform>().each(
			[](ObjMorphLoader& obj, Transform& trans) {
				obj.Update(Time::dt);
			}
		);
		m_reg.view<Player, PhysBody>().each(
			[](Player& p, PhysBody& body) {
				p.Update(body);
			}
		);
		m_reg.view<Spawner, Transform>().each(
			[&](Spawner& spawn, Transform& trans) {
				spawn.Update(&m_reg, trans.GetGlobalPosition());
			}
		);

		if (m_world != nullptr) {
			m_world->stepSimulation(Time::dt, 10);

			m_reg.view<PhysBody, Transform>().each(
				[](PhysBody& phys, Transform& trans) {
					if (phys.IsDynamic() || phys.Changed())
						trans.SetTransform(phys.GetTransform());
				}
			);
		}

		m_reg.view<Player, Transform>().each(
			[](Player& p, Transform& trans) {
				p.LateUpdate(trans);
			}
		);

		m_effects.Update();
	}

	LateUpdate();

	//always render
	if (m_camCount < 1)
		m_camCount = 1;
	else if (m_camCount > 4)
		m_camCount = 4;

	Sprite::BeginUIDraw(10, m_camCount);

	Rendering::Update(&m_reg, m_camCount, m_paused);

	//once pause buffer works, we can move this or smt
	m_frameEffects.Draw();

	Sprite::PerformUIDraw(m_camCount);

	if (m_paused)   if (m_pauseSprite.IsValid()) {
		glViewport(0, 0, BackEnd::GetWidth(), BackEnd::GetHeight());

		static const glm::mat4 pauseMat = glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, -100, 1
		);

		m_pauseSprite.DrawSingle(Rendering::orthoVP.GetViewProjection(), pauseMat);
		//Rendering::DrawPauseScreen(m_pauseSprite);
	}
}

void Scene::QueueSceneChange(size_t index) {
	if (m_nextScene >= m_scenes.size())
		return;

	m_nextScene = index;
	m_doSceneChange = true;
}

void Scene::doSceneChange(GLFWwindow* window) {
	if (m_doSceneChange == false)
		return;

	m_doSceneChange = false;
	m_activeScene->Exit();
	m_activeScene = m_scenes[m_nextScene]->Reattach();
	glfwSetWindowTitle(window, m_activeScene->GetName().c_str());
}

void Scene::UnloadScenes()
{
	m_activeScene = nullptr;
	while (m_scenes.size()) {
		delete m_scenes[0];
		m_scenes.erase(m_scenes.begin());
	}
}
