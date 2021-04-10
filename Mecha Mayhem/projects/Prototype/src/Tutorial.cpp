#include "Tutorial.h"
#include "LeaderBoard.h"

Tutorial::Tutorial(const std::string& name, const glm::vec3& gravity)
	: Scene(name, gravity, true)
{
	m_frameEffects.SetShadowVP(-35, 35, 40, -60, glm::vec3(0, 0, -30));
	m_frameEffects.GetSun()._shadowBiasMax = 0.0015f;
	m_frameEffects.GetSun()._lightDirection = glm::vec4(-4, -5, -4, 0);
}

void Tutorial::Init(int width, int height)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "tutorial";
	m_colliders.Init(m_world, input, false, false);
	/*std::cout << "filename: " + input + "\n";
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";*/

	m_camCount = 4;

	/// Creating Entities
	/// 
	/// //players created in reattach
	
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, BLM::GLMQuat, glm::vec3(0, 1, -12))
			.GetBody()->setMassProps(0, btVector3(0, -1, 0));
		ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 0).SetMaxHealth(100)
			.SetSpawn(glm::vec3(4, 1, -59)).SetRotation(glm::radians(180.f), 0).TakeDamage(76);
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, BLM::GLMQuat, glm::vec3(0, 2.5f, -20))
			.GetBody()->setMassProps(0, btVector3(0, -1, 0));
		ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 0).SetMaxHealth(100)
			.SetSpawn(glm::vec3(-4, 1, -59)).SetRotation(glm::radians(180.f), 0);
	}

	m_colliders.GenerateSpawners();
	
	/*{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<Spawner>(entity).Init(0.3f, 2.5f).SetBounds(1, 5);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 0, -18));
	}*/
	//dummies
	for (int i(0); i < 3; ++i) {
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, BLM::GLMQuat, glm::vec3(-7.5f, 1, i * 7.5f - 55))
				.GetBody()->setMassProps(0, btVector3(0, -1, 0));
			ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 0).SetMaxHealth(100)
				.SetSpawn(glm::vec3(-7.5f, 1, i * 7.5f - 55)).SetRotation(glm::radians(90.f), 0);
			dummies[i].dummy = entity;
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Sprite>(entity);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(-9.f, 2.5f, i * 7.5f - 55.40f))
				.SetRotation(glm::angleAxis(glm::radians(90.f), BLM::GLMup));
			dummies[i].digit2 = entity;
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Sprite>(entity);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(-9.f, 2.5f, i * 7.5f - 54.60f))
				.SetRotation(glm::angleAxis(glm::radians(90.f), BLM::GLMup));
			dummies[i].digit1 = entity;
		}
		SetDigits(i, 0);

		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, BLM::GLMQuat, glm::vec3(7.5f, 1, i * 7.5f - 55))
				.GetBody()->setMassProps(0, btVector3(0, -1, 0));
			ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 0).SetMaxHealth(100)
				.SetSpawn(glm::vec3(7.5f, 1, i * 7.5f - 55)).SetRotation(glm::radians(270.f), 0);
			dummies[i + 3].dummy = entity;
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Sprite>(entity);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(9.f, 2.5f, i * 7.5f - 54.60f))
				.SetRotation(glm::angleAxis(glm::radians(270.f), BLM::GLMup));
			dummies[i + 3].digit2 = entity;
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Sprite>(entity);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(9.f, 2.5f, i * 7.5f - 55.40f))
				.SetRotation(glm::angleAxis(glm::radians(270.f), BLM::GLMup));
			dummies[i + 3].digit1 = entity;
		}
		SetDigits(i + 3, 0);
	}

	//map
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<MultiTextObj>(entity).LoadMesh("maps/tutorialMap.obj");
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 0, 0));
	}

	//details
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/light.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 5, 4));
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/LightDrone.obj", true);
		lightDrone = entity;
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/CameraDrone.obj", true);
		cameraDrone = entity;
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/SpeakerDrone.obj", true);
		speakerDrone = entity;
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

	m_frameEffects.Init();

	Player::SetCamDistance(camDistance);

	Player::SetSkyPos(glm::vec3(0, 25, -45));

	m_pauseSprite = { "Pause.png", 8.952f, 3 };
}

void Tutorial::SetDigits(int index, int number)
{
	if (index < 0 || index > 5)	return;
	if (number > 99) {
		ECS::GetComponent<Sprite>(dummies[index].digit2).Init("num/9.png", -0.75f, 1.f);
		ECS::GetComponent<Sprite>(dummies[index].digit1).Init("num/9.png", -0.75f, 1.f);
		return;
	}

	int val = number / 10;
	ECS::GetComponent<Sprite>(dummies[index].digit2).Init(
		"num/" + std::to_string(number - val * 10) + ".png", -0.75f, 1.f);
	ECS::GetComponent<Sprite>(dummies[index].digit1).Init("num/" + std::to_string(val) + ".png", -0.75f, 1.f);
}

void Tutorial::Update()
{
	/*if (Input::GetKeyDown(KEY::F)) {
		if (++m_camCount > 4)	m_camCount = 1;
		if (m_camCount == 2) {
			ECS::GetComponent<Camera>(cameraEnt[0]).ResizeWindow(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
			if (cameraEnt[1] != entt::null)
			{
				ECS::GetComponent<Camera>(cameraEnt[1]).ResizeWindow(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
			}
			Player::SetUIAspect(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
		}
		else {
			ECS::GetComponent<Camera>(cameraEnt[0]).ResizeWindow(BackEnd::GetWidth(), BackEnd::GetHeight());
			if (cameraEnt[1] != entt::null)
			{
				ECS::GetComponent<Camera>(cameraEnt[1]).ResizeWindow(BackEnd::GetWidth(), BackEnd::GetHeight());
			}
			Player::SetUIAspect(BackEnd::GetWidth(), BackEnd::GetHeight());
		}
	}*/

	bool winner = false;
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


		if (ControllerInput::GetButtonDown(BUTTON::SELECT, CONUSER(i))) {
			winner = true;
			if (m_paused) {
				m_paused = false;
				//remove effects or smt
				m_frameEffects.RemoveEffect(0);
			}
		}
	}

	if (m_paused) {
		for (int i(0), temp(0); i < 4; ++i) {
			if (LeaderBoard::players[i].user != CONUSER::NONE) {
				LeaderBoard::players[i].sensitivity = ECS::GetComponent<Player>(bodyEnt[temp]).EditSensitivity();
				++temp;
			}
		}
		return;
	}

	/*auto &p1 = ECS::GetComponent<Player>(bodyEnt1);
	auto &p2 = ECS::GetComponent<Player>(bodyEnt2);
	auto &p3 = ECS::GetComponent<Player>(bodyEnt3);
	auto &p4 = ECS::GetComponent<Player>(bodyEnt4);

	p1.GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt1),
		ECS::GetComponent<Transform>(Head1),
		ECS::GetComponent<Transform>(cameraEnt1)
	);

	p2.GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt2),
		ECS::GetComponent<Transform>(Head2),
		ECS::GetComponent<Transform>(cameraEnt2)
	);

	p3.GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt3),
		ECS::GetComponent<Transform>(Head3),
		ECS::GetComponent<Transform>(cameraEnt3)
	);

	p4.GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt4),
		ECS::GetComponent<Transform>(Head4),
		ECS::GetComponent<Transform>(cameraEnt4)
	);*/

	for (int i(0); i < 6; ++i) {
		auto& p = ECS::GetComponent<Player>(dummies[i].dummy);

		short lastTemp = dummies[i].lastHealth - dummies[i].currHealth;
		dummies[i].currHealth = p.GetHealth();
		short temp = dummies[i].lastHealth - dummies[i].currHealth;

		if (temp != lastTemp) {
			SetDigits(i, temp);
			dummies[i].timer = 2.5f;
		}

		if (dummies[i].timer > 0) {
			dummies[i].timer -= Time::dt;
			if (dummies[i].timer <= 0) {
				dummies[i].timer = 0;
				SetDigits(i, 0);

				p.GainHealth(100);
				dummies[i].lastHealth = 100;
				dummies[i].currHealth = 100;
			}
		}
	}

	for (int i(0); i < LeaderBoard::playerCount; ++i) {
		auto& p = ECS::GetComponent<Player>(bodyEnt[i]);
		p.GetInput(
			ECS::GetComponent<PhysBody>(bodyEnt[i]),
			ECS::GetComponent<Transform>(Head[i]),
			ECS::GetComponent<Transform>(cameraEnt[i])
		);

		if (ControllerInput::GetButtonDown(BUTTON::SELECT, p.GetUser()))
			winner = true;
	}

	if (m_timer > 0) {
		m_timer -= Time::dt;
		if (m_timer <= 0) {
			m_timer = 0.f;

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
			QueueSceneChange(0);
		}
	}
	else if (winner) {
		//AudioEngine::Instance().GetEvent("reload").Restart();
		SoundEventManager::Play(SoundEventManager::SOUND::RELOAD,
			ECS::GetComponent<Transform>(bodyEnt[0]).GetGlobalPosition());

		for (int i(0), temp(0); i < 4; ++i) {
			if (LeaderBoard::players[i].user != CONUSER::NONE) {
				LeaderBoard::players[i].score = ECS::GetComponent<Player>(bodyEnt[temp]).GetScore();
				++temp;
			}
		}

		m_reg.view<Player>().each(
			[](Player& p) {
				p.MakeInvincible(true);
				p.GainHealth(100);
			}
		);

		m_timer = 1.f;
	}

	ECS::GetComponent<Transform>(lightDrone).SetPosition(dronePath.Update(Time::dt).GetPosition()).SetRotation(dronePath.GetLookingForwards(0.5f));
	ECS::GetComponent<Transform>(cameraDrone).SetPosition(dronePath2.Update(Time::dt).GetPosition()).SetRotation(dronePath2.GetLookingForwards(0.5f));
	ECS::GetComponent<Transform>(speakerDrone).SetPosition(dronePath3.Update(Time::dt).GetPosition()).SetRotation(dronePath3.GetLookingForwards(0.5f));
}

void Tutorial::LateUpdate()
{
	//auto& trans = ECS::GetComponent<Transform>(Head[0]);
	//SoundEventManager::UpdatePosition(trans.GetGlobalPosition(), -trans.GetForwards());
}

void Tutorial::DrawOverlay()
{

	if (m_paused) {
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

Scene* Tutorial::Reattach()
{
	Scene::Reattach();

	Rendering::DefaultColour = glm::vec4(0.75f, 0.75f, 0.75f, 1.f);
	Rendering::LightsColour[0] = glm::vec3(20.f);
	Rendering::LightCount = 2;
	Rendering::LightsPos[0] = glm::vec3(0, 10, -42.5f);
	Rendering::LightsPos[1] = glm::vec3(0, 5, 0);
	//fix lights
	FrameEffects::SetLights(Rendering::LightsPos, Rendering::LightsColour, Rendering::LightCount);

	m_camCount = LeaderBoard::playerCount;

	for (int temp(3), i(LeaderBoard::playerCount); temp >= 0; --temp) {
		if (LeaderBoard::players[temp].user == CONUSER::NONE)
			continue;

		--i;
		cameraEnt[i] = ECS::CreateEntity();
		if (m_camCount == 2)
			ECS::AttachComponent<Camera>(cameraEnt[i]).Setfar(100.f).SetFovDegrees(60.f).ResizeWindow(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
		else
			ECS::AttachComponent<Camera>(cameraEnt[i]).Setfar(100.f).SetFovDegrees(60.f).ResizeWindow(BackEnd::GetWidth(), BackEnd::GetHeight());

		bodyEnt[i] = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(bodyEnt[i]).CreatePlayer(bodyEnt[i], BLM::GLMQuat, glm::vec3(0, 1.5f, 0));
		ECS::AttachComponent<Player>(bodyEnt[i]).Init(
			LeaderBoard::players[temp].user, LeaderBoard::players[temp].model, LeaderBoard::players[temp].colour, i
		).SetRotation(glm::radians(180.f), 0).SetSpawn(glm::vec3(0, 1.5f, 0)).SetSensitivity(LeaderBoard::players[temp].sensitivity);

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
	Player::SetSkyPos(glm::vec3(0, 25, -45));

	//SoundEventManager::ThreeDSounds = true;

	return this;
}
