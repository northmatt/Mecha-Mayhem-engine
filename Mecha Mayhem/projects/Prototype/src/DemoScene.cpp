#include "DemoScene.h"
#include "LeaderBoard.h"

void DemoScene::Init(int width, int height)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "playtestmap";
	m_colliders.Init(m_world, input, false, false);
	/*std::cout << "filename: " + input + "\n";
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";*/

	m_camCount = 4;

	//map
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<MultiTextObj>(entity).LoadMesh("maps/MapUntextured.obj");
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -30.f, 0)).SetScale(4.f);
	}

	m_colliders.GenerateSpawners();

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
				if (BackEnd::GetFullscreen())	BackEnd::SetTabbed();
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
						m_frameEffects[0]->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
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
		auto& body = ECS::GetComponent<PhysBody>(bodyEnt[i]);
		p.GetInput(
			body,
			ECS::GetComponent<Transform>(Head[i]),
			ECS::GetComponent<Transform>(cameraEnt[i])
		);

		Rendering::LightsPos[2 + i] = BLM::BTtoGLM(body.GetTransform().getOrigin()) - BLM::GLMup;

		if (!p.IsAlive()) {
			//check if respawn time is about to say dead
			if (p.RespawnDelayTest(Time::dt)) {
				std::vector<glm::vec3> tests = {};
				for (int x(0); x < LeaderBoard::playerCount; ++x) {
					if (x == i)	continue;

					//only add to list if alive
					if (ECS::GetComponent<Player>(bodyEnt[x]).IsAlive())
						tests.push_back(BLM::BTtoGLM(ECS::GetComponent<PhysBody>(bodyEnt[x]).GetTransform().getOrigin()));
				}
				m_colliders.SetSpawnAvoid(p, tests, 40.f);
			}
		}

		if (p.GetScore() >= killGoal)
			winner = true;
	}

	if (m_timer > 0) {
		m_timer -= Time::dt;
		if (m_timer <= 0) {
			m_timer = 0;

			//does the reset stuff here
			m_reg = entt::registry();

			for (int i(0); i < LeaderBoard::playerCount; ++i) {
				bodyEnt[i] = Head[i] = cameraEnt[i] = entt::null;
			}

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

			Init(BackEnd::GetWidth(), BackEnd::GetHeight());
			QueueSceneChange(3);
		}
	}
	else if (winner) {
		for (int i(0), temp(0); i < 4; ++i) {
			if (LeaderBoard::players[i].user != CONUSER::NONE) {
				ECS::GetComponent<Player>(bodyEnt[temp]).GameEnd();
				LeaderBoard::players[i].score = ECS::GetComponent<Player>(bodyEnt[temp]).GetScore();
				++temp;
			}
		}
		m_timer = 5.f;
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

	m_frameEffects.Resize(BackEnd::GetWidth(), BackEnd::GetHeight());

	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;
	Rendering::DefaultColour = glm::vec4(0.75f, 0.75f, 0.75f, 1.f);
	Rendering::LightsColour[0] = glm::vec3(200.f);
	Rendering::LightCount = 2 + LeaderBoard::playerCount;
	Rendering::LightsPos[0] = glm::vec3(20.5, 10, -21.8f);
	Rendering::LightsPos[1] = glm::vec3(20.5, 10, -21.8f);
	Rendering::AmbientStrength = 1.f;

	m_camCount = LeaderBoard::playerCount;

	std::vector<glm::vec3> spawntests = {};
	//prob wanna improve on this
	{
		if (LeaderBoard::playerCount > 2) {
			killGoal = 10;

			for (int i(0); i < 4;) {
				glm::vec3 tempPos;
				switch (rand() % 4) {
				case 0:	tempPos = glm::vec3(70, -20, 30);	break;
				case 1:	tempPos = glm::vec3(-30, -20, 20);	break;
				case 2:	tempPos = glm::vec3(20, -20, -30);	break;
				case 3:	tempPos = glm::vec3(25, -20, 75);	break;
				}

				bool valid = true;
				for (int x(0); x < spawntests.size(); ++x) {
					if (spawntests[x] == tempPos) {
						valid = false;
						break;
					}
				}

				if (valid) {
					spawntests.push_back(tempPos);
					++i;
				}
			}
		}
		else {
			killGoal = 5;

			if (rand() % 50 < 25) {
				for (int i(0); i < 2;) {
					glm::vec3 tempPos;
					if (rand() % 50 < 25)	tempPos = glm::vec3(70, -20, 30);
					else	tempPos = glm::vec3(-30, -20, 20);

					bool valid = true;
					for (int x(0); x < spawntests.size(); ++x) {
						if (spawntests[x] == tempPos) {
							valid = false;
							break;
						}
					}

					if (valid) {
						spawntests.push_back(tempPos);
						++i;
					}
				}
			}
			else {
				for (int i(0); i < 2;) {
					glm::vec3 tempPos;
					if (rand() % 50 < 25) tempPos = glm::vec3(20, -20, -30);
					else	tempPos = glm::vec3(25, -20, 75);

					bool valid = true;
					for (int x(0); x < spawntests.size(); ++x) {
						if (spawntests[x] == tempPos) {
							valid = false;
							break;
						}
					}

					if (valid) {
						spawntests.push_back(tempPos);
						++i;
					}
				}
			}
		}
	}

	for (int temp(3), i(LeaderBoard::playerCount); temp >= 0; --temp) {
		if (LeaderBoard::players[temp].user == CONUSER::NONE)
			continue;

		--i;
		cameraEnt[i] = ECS::CreateEntity();
		if (m_camCount == 2)
			ECS::AttachComponent<Camera>(cameraEnt[i]).SetFovDegrees(60.f).ResizeWindow(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
		else
			ECS::AttachComponent<Camera>(cameraEnt[i]).SetFovDegrees(60.f).ResizeWindow(BackEnd::GetWidth(), BackEnd::GetHeight());

		bodyEnt[i] = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(bodyEnt[i]).CreatePlayer(bodyEnt[i], BLM::GLMzero,
			m_colliders.SetSpawnNear(
				ECS::AttachComponent<Player>(bodyEnt[i]).Init(
					LeaderBoard::players[temp].user, LeaderBoard::players[temp].model, i).SetRotation(glm::radians(180.f), 0),
				spawntests[i], 20
			)
		);

		Head[i] = ECS::CreateEntity();
		ECS::GetComponent<Transform>(Head[i]).SetPosition(glm::vec3(0, 0.75f, 0)).
			ChildTo(bodyEnt[i]).SetUsingParentScale(false);
		ECS::GetComponent<Transform>(cameraEnt[i]).SetPosition(glm::vec3(0, 0, camDistance)).
			ChildTo(Head[i]).SetUsingParentScale(false);
	}

	if (m_camCount == 2)
		Player::SetUIAspect(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
	else
		Player::SetUIAspect(BackEnd::GetWidth(), BackEnd::GetHeight());

	Player::SetCamDistance(camDistance);
	Player::SetSkyPos(glm::vec3(0, 50, -45));

	return this;
}
