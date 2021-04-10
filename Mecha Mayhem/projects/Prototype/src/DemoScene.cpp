#include "DemoScene.h"
#include "LeaderBoard.h"

DemoScene::DemoScene(const std::string& name, const glm::vec3& gravity)
	: Scene(name, gravity, true)
{
	m_frameEffects.SetShadowVP(-70, 70, 40, -60, glm::vec3(20.5f - 0.f, 0, 21.8f + 5.f));
}

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
	Rendering::LightsPos[0] = glm::vec3(20.5f, -1, 21.8f);
	Rendering::LightsPos[1] = glm::vec3(20.5f, 3, 21.8f);

	Rendering::hitboxes = &m_colliders;
	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;

	m_frameEffects.Init();

	Player::SetUIAspect(width, height);
	Player::SetCamDistance(camDistance);

	Player::SetSkyPos(glm::vec3(20.5f, 10, 21.8f));

	m_pauseSprite = { "Pause.png", 8.952f, 3 };
	m_colonSprite = { "colon.png", 0.3736f, 1.6667f };
}

void DemoScene::Update()
{
	for (int i(0); i < 4; ++i) {
		if (ControllerInput::GetButtonDown(BUTTON::START, CONUSER(i))) {
			if ((m_timer > 0 || m_paused) && ControllerInput::GetButton(BUTTON::RB, CONUSER(i))) {
				if (BackEnd::GetFullscreen())	BackEnd::SetTabbed();
				else							BackEnd::SetFullscreen();
			}
			else if (m_timer == 0) {
				if (m_paused) {
					m_paused = false;
					//remove effects or smt
					m_frameEffects.RemoveEffect(0);
				}
				else {
					m_paused = true;
					//add effects or smt
					m_frameEffects.InsertEffect(new PixelEffect(), 0);
					m_frameEffects[0]->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					((PixelEffect*)(m_frameEffects[0]))->SetPixelCount(128);
				}
			}
		}

		if (m_timer == 0) if (LeaderBoard::players[i].user != CONUSER::NONE) {
			if (ControllerInput::ControllerDisconnected(CONUSER(i))) {
				std::cout << "-Controller " << i + 1 << " disconnected\n";
				m_paused = true;
				//add effects or smt
				m_frameEffects.InsertEffect(new PixelEffect(), 0);
				m_frameEffects[0]->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
				((PixelEffect*)(m_frameEffects[0]))->SetPixelCount(128);
			}
		}
	}

	bool winner = false;
	if (m_paused) {
		int allHolding = 0;
		for (int i(0), temp(0); i < 4; ++i) {
			if (LeaderBoard::players[i].user != CONUSER::NONE) {
				LeaderBoard::players[i].sensitivity = ECS::GetComponent<Player>(bodyEnt[temp]).EditSensitivity();
				allHolding += ControllerInput::GetButton(BUTTON::SELECT, LeaderBoard::players[i].user);
				++temp;
			}
		}

		if (allHolding == LeaderBoard::playerCount) {
			m_paused = false;
			//remove effects or smt
			m_frameEffects.RemoveEffect(0);
			winner = true;
		}
		else			return;
	}

	for (int i(0); i < LeaderBoard::playerCount; ++i) {
		auto& p = ECS::GetComponent<Player>(bodyEnt[i]);
		p.GetInput(
			ECS::GetComponent<PhysBody>(bodyEnt[i]),
			ECS::GetComponent<Transform>(Head[i]),
			ECS::GetComponent<Transform>(cameraEnt[i])
		);

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

	if (m_gameTimer > 0) {
		m_gameTimer -= Time::dt;
		if (m_gameTimer <= 0) {
			winner = true;
		}
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
		//we update the colour correction
		else if (m_frameEffects[0]) {
			((ColourCorrection*)m_frameEffects[0])->SetIntensity(1.f - (m_timer / winWaitTime));
		}
	}
	else if (winner) {
		for (int i(0), temp(0); i < 4; ++i) {
			if (LeaderBoard::players[i].user != CONUSER::NONE) {
				auto& p = ECS::GetComponent<Player>(bodyEnt[temp]);

				p.GameEnd();
				LeaderBoard::players[i].score = p.GetScore();

				if (!p.IsAlive()) {
					std::vector<glm::vec3> tests = {};
					for (int x(0); x < LeaderBoard::playerCount; ++x) {
						if (x == i)	continue;

						//only add to list if alive
						if (ECS::GetComponent<Player>(bodyEnt[x]).IsAlive())
							tests.push_back(BLM::BTtoGLM(ECS::GetComponent<PhysBody>(bodyEnt[x]).GetTransform().getOrigin()));
					}
					m_colliders.SetSpawnAvoid(p, tests, 20.f);
				}
				++temp;
			}
		}
		SoundEventManager::Create(SoundEventManager::SOUND::GAMEMUSIC).SetParameter("Music Volume", 0.f);
		SoundEventManager::Play(SoundEventManager::SOUND::MATCHEND);
		m_timer = winWaitTime;
		if (LeaderBoard::timedGoal)
			m_gameTimer = 0.f;

		//at end of game, add colour correction;
		if (m_frameEffects[0] == nullptr) {
			m_frameEffects.InsertEffect(new ColourCorrection(), 0);
			m_frameEffects[0]->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
			((ColourCorrection*)m_frameEffects[0])->SetIntensity(0.f);
			((ColourCorrection*)m_frameEffects[0])->SetCube(gameEndCube);
		}
	}
}

void DemoScene::LateUpdate()
{
	//this makes the light match the player's position

	for (int i(0); i < LeaderBoard::playerCount; ++i) {
		Rendering::LightsPos[2 + i] = ECS::GetComponent<Transform>(bodyEnt[i]).GetGlobalPosition() - BLM::GLMup * 0.5f;
	}
	//fix lights
	FrameEffects::SetLights(Rendering::LightsPos, Rendering::LightsColour, Rendering::LightCount);
}

void DemoScene::DrawOverlay()
{
	//drawTimer if timed match
	if (m_gameTimer >= 0) {
		glViewport(0, 0, BackEnd::GetWidth(), BackEnd::GetHeight());

		//colon position
		float xPos = 0.75f;
		//10 minutes
		if (m_gameTimer >= 600.f)
			xPos = 0;
		glm::mat4 timerMat = glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			xPos, m_yPos, -99.9f, 1
		);
		Sprite::BeginUIDraw(5, 1);

		m_colonSprite.DrawToUI(Rendering::orthoVP.GetViewProjection(), timerMat, 0);

		//get minutes
		int min1 = m_gameTimer / 60.f;

		//get seconds
		int sec1 = ceilf(m_gameTimer - floorf(m_gameTimer / 60.f) * 60.f);
		min1 += sec1 == 60;
		int min2 = min1 / 10;
		sec1 *= sec1 != 60;
		int sec2 = sec1 / 10;
		min1 -= (min2 * 10);
		sec1 -= (sec2 * 10);

		//draw digits here
		if (min2) {
			timerMat[3].x = xPos + 2.25f;
			Player::m_digits[min2].DrawToUI(Rendering::orthoVP.GetViewProjection(), timerMat, 0);
		}
		timerMat[3].x = xPos + 0.85f;
		Player::m_digits[min1].DrawToUI(Rendering::orthoVP.GetViewProjection(), timerMat, 0);
		timerMat[3].x = xPos - 0.85f;
		Player::m_digits[sec2].DrawToUI(Rendering::orthoVP.GetViewProjection(), timerMat, 0);
		timerMat[3].x = xPos - 2.25f;
		Player::m_digits[sec1].DrawToUI(Rendering::orthoVP.GetViewProjection(), timerMat, 0);

		Sprite::PerformUIDraw(1);
	}

	if (m_paused) {
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

Scene* DemoScene::Reattach()
{
	Scene::Reattach();

	Rendering::DefaultColour = glm::vec4(0.75f, 0.75f, 0.75f, 1.f);
	Rendering::LightsColour[0] = glm::vec3(20.f);
	Rendering::LightCount = 2 + LeaderBoard::playerCount;
	Rendering::LightsPos[0] = glm::vec3(20.5f, -1, 21.8f);
	Rendering::LightsPos[1] = glm::vec3(20.5f, 3, 21.8f);
	//fix lights
	FrameEffects::SetLights(Rendering::LightsPos, Rendering::LightsColour, Rendering::LightCount);

	m_camCount = LeaderBoard::playerCount;
	if (LeaderBoard::timedGoal) {
		m_gameTimer = LeaderBoard::scoreGoal * 60.f;
		if (LeaderBoard::playerCount == 2)
			m_yPos = -6.2f;
		else
			m_yPos = 8.85f;
	}
	else
		killGoal = LeaderBoard::scoreGoal;

	std::vector<float> spawntests = {};
	std::vector<glm::vec3> spawnPos = {
			glm::vec3(60, -20, 20),
			glm::vec3(-30, -20, 30),
			glm::vec3(20, -20, -40),
			glm::vec3(20, -20, 80)
	};
	//prob wanna improve on this
	{
		int offset = 0;
		if (LeaderBoard::playerCount <= 2) {
			//only in 2-player mode do we add the chance for offset
			offset = (rand() % 2) * 2;
		}
		for (int i(0); i < LeaderBoard::playerCount;) {
			int tempPos = offset + rand() % LeaderBoard::playerCount;

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

	for (int temp(3), i(LeaderBoard::playerCount); temp >= 0; --temp) {
		if (LeaderBoard::players[temp].user == CONUSER::NONE)
			continue;

		--i;
		cameraEnt[i] = ECS::CreateEntity();
		if (m_camCount == 2)
			ECS::AttachComponent<Camera>(cameraEnt[i]).Setfar(300.f).SetFovDegrees(60.f).ResizeWindow(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
		else
			ECS::AttachComponent<Camera>(cameraEnt[i]).Setfar(300.f).SetFovDegrees(60.f).ResizeWindow(BackEnd::GetWidth(), BackEnd::GetHeight());

		bodyEnt[i] = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(bodyEnt[i]).CreatePlayer(bodyEnt[i], BLM::GLMQuat,
			m_colliders.SetSpawnNear(
				ECS::AttachComponent<Player>(bodyEnt[i]).Init(
					LeaderBoard::players[temp].user, LeaderBoard::players[temp].model, LeaderBoard::players[temp].colour, i
					).SetRotation(glm::radians(180.f), 0).SetSensitivity(LeaderBoard::players[temp].sensitivity),
				spawnPos[spawntests[i]], 25.f
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
	Player::SetSkyPos(glm::vec3(20.5f, 10, 21.8f));

	AudioEvent& music = SoundEventManager::Create(SoundEventManager::SOUND::GAMEMUSIC);
	music.SetParameter("Music Volume", 1.f);
	music.Restart();


	return this;
}

void DemoScene::ImGuiFunc()
{
	Scene::ImGuiFunc();

	for (int i(0); i < 4; ++i) {
		if (bodyEnt[i] != entt::null)
			if (ImGui::Button(("Give player " + std::to_string(i + 1) + " 1 kill").c_str()))
				ECS::GetComponent<Player>(bodyEnt[i]).GivePoints(1);
	}
}
