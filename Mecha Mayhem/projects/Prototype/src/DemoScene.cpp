#include "DemoScene.h"
#include "LeaderBoard.h"

void DemoScene::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "playtestmap";
	std::cout << "filename: " + input + "\n";
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";

	width = windowWidth;
	height = windowHeight;

	m_camCount = 4;

	//map
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<MultiTextObj>(entity).LoadMesh("maps/MapUntextured.obj");
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -30.f, 0)).SetScale(4.f);
	}

	/// End of creating entities
	Rendering::DefaultColour = glm::vec4(0.75f, 0.75f, 0.75f, 1.f);
	Rendering::LightCount = 2;
	Rendering::LightsColour[0] = glm::vec3(200.f);
	Rendering::LightsPos[0] = glm::vec3(0, 10, -42.5f);
	Rendering::LightsPos[1] = glm::vec3(0, 2, 0);

	Rendering::hitboxes = &m_colliders;
	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;

	m_frameEffects.Init(width, height);

	Player::SetUIAspect(width, height);
	Player::SetCamDistance(camDistance);

	Player::SetSkyPos(glm::vec3(0, 50, -45));

	m_pauseSprite = Sprite("Pause.png", 8.952f, 3);
}

void DemoScene::Update()
{
	for (size_t i(0); i < 4; ++i) {
		if (ControllerInput::GetButtonDown(BUTTON::START, CONUSER(i))) {
			if (ControllerInput::GetButton(BUTTON::RB, CONUSER(i))) {
				if (BackEnd::GetFullscreen())	BackEnd::SetTabbed(width, height);
				else							BackEnd::SetFullscreen();
			}
			else {
				if (m_paused) {
					m_paused = false;
					//remove effects or smt
					m_frameEffects.RemoveEffect(0);
				}
				else {
					m_paused = true;
					//add effects or smt
					if (m_frameEffects.size() == 0) {
						m_frameEffects.AddEffect(new PixelEffect());
						m_frameEffects[0]->Init(width, height);
						((PixelEffect*)(m_frameEffects[0]))->SetPixelCount(64);
					}
				}
			}
		}
	}

	if (m_paused)
		return;

	bool winner = false;
	for (int i(0); i < LeaderBoard::playerCount; ++i) {
		auto& p = ECS::GetComponent<Player>(bodyEnt[i]);
		p.GetInput(
			ECS::GetComponent<PhysBody>(bodyEnt[i]),
			ECS::GetComponent<Transform>(Head[i]),
			ECS::GetComponent<Transform>(cameraEnt[i])
		);

		if (p.GetScore() >= killGoal)
			winner = true;
	}

	if (winner) {
		for (int i(0), temp(0); i < 4; ++i) {
			if (LeaderBoard::players[i].user != CONUSER::NONE) {
				LeaderBoard::players[i].score = ECS::GetComponent<Player>(bodyEnt[temp]).GetScore();
				bodyEnt[temp] = Head[temp] = cameraEnt[temp] = entt::null;
				++temp;
			}
		}


		//does the reset stuff here
		m_reg = entt::registry();

		btVector3 grav = m_world->getGravity();
		for (int i = m_world->getNumCollisionObjects() - 1; i >= 0; --i) {
			btCollisionObject* obj = m_world->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			m_world->removeCollisionObject(obj);
			delete obj;
		}

		delete m_world;
		m_world = new btDiscreteDynamicsWorld(
			_dispatcher, _broadphase, _solver, _collisionConfiguration);
		m_world->setGravity(grav);

		m_colliders.Clear();

		Init(BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);
		QueueSceneChange(3);
	}
}

Scene* DemoScene::Reattach()
{
	ECS::AttachRegistry(&m_reg);
	if (m_world) {
		PhysBody::Init(m_world);
		ECS::AttachWorld(m_world);
		std::string input = "playtestmap";
		std::cout << "filename: " + input + "\n";
		if (!m_colliders.Init(m_world, input, false, false))
			std::cout << input + " failed to load, no collision boxes loaded\n";
		Rendering::hitboxes = &m_colliders;
	}

	m_frameEffects.Resize(BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);

	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;
	Rendering::DefaultColour = glm::vec4(0.75f, 0.75f, 0.75f, 1.f);
	Rendering::LightsColour[0] = glm::vec3(200.f);
	Rendering::LightCount = 2;
	Rendering::LightsPos[0] = glm::vec3(0, 10, -42.5f);
	Rendering::LightsPos[1] = glm::vec3(0, 2, 0);
	Rendering::AmbientStrength = 1.f;

	m_camCount = LeaderBoard::playerCount;

	for (int temp(3), i(LeaderBoard::playerCount); temp >= 0; --temp) {
		if (LeaderBoard::players[temp].user == CONUSER::NONE)
			continue;

		--i;
		cameraEnt[i] = ECS::CreateEntity();
		if (m_camCount == 2)
			ECS::AttachComponent<Camera>(cameraEnt[i]).SetFovDegrees(60.f).ResizeWindow(width / 2, height);
		else
			ECS::AttachComponent<Camera>(cameraEnt[i]).SetFovDegrees(60.f).ResizeWindow(width, height);

		bodyEnt[i] = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(bodyEnt[i]).CreatePlayer(bodyEnt[i], startQuat, glm::vec3(0, 1.5f, 0));
		ECS::AttachComponent<Player>(bodyEnt[i]).Init(
			LeaderBoard::players[temp].user, LeaderBoard::players[temp].model, i).SetRotation(glm::radians(180.f), 0);

		Head[i] = ECS::CreateEntity();
		ECS::GetComponent<Transform>(Head[i]).SetPosition(glm::vec3(0, 0.75f, 0)).
			ChildTo(bodyEnt[i]).SetUsingParentScale(false);
		ECS::GetComponent<Transform>(cameraEnt[i]).SetPosition(glm::vec3(0, 0, camDistance)).
			ChildTo(Head[i]).SetUsingParentScale(false);
	}

	if (m_camCount == 2)
		Player::SetUIAspect(width / 2, height);
	else
		Player::SetUIAspect(width, height);

	Player::SetCamDistance(camDistance);
	Player::SetSkyPos(glm::vec3(0, 50, -45));

	return this;
}
