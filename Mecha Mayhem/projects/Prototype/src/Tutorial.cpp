#include "Tutorial.h"
#include "LeaderBoard.h"
#include "Effects/Post/BloomEffect.h"

void Tutorial::Init(int width, int height)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "tutorial";
	std::cout << "filename: " + input + "\n";
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";

	m_camCount = 4;

	/// Creating Entities
	/// 
	/// //players created in reattach
	
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, BLM::GLMQuat, glm::vec3(0, 1, -12))
			.GetBody()->setMassProps(0, btVector3(0, -1, 0));
		ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 69)
			.SetSpawn(glm::vec3(4, 1, -59)).SetRotation(glm::radians(180.f), 0);// .SetMaxHealth(5);
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, BLM::GLMQuat, glm::vec3(0, 2.5f, -20))
			.GetBody()->setMassProps(0, btVector3(0, -1, 0));
		ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 69)
			.SetSpawn(glm::vec3(-4, 1, -59)).SetRotation(glm::radians(180.f), 0);// .SetMaxHealth(10);
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
			ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 69).SetSpawn(glm::vec3(-7.5f, 1, i * 7.5f - 55))
				.SetRotation(glm::radians(90.f), 0);
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, BLM::GLMQuat, glm::vec3(7.5f, 1, i * 7.5f - 55))
				.GetBody()->setMassProps(0, btVector3(0, -1, 0));
			ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 69).SetSpawn(glm::vec3(7.5f, 1, i * 7.5f - 55))
				.SetRotation(glm::radians(270.f), 0);
		}
		/*{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Spawner>(entity).Init(0.3f, 5.f);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(5.f, 0, i * 7.5f - 55));
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Spawner>(entity).Init(0.3f, 5.f);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(-5.f, 0, i * 7.5f - 55));
		}*/
	}

	//map
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("maps/tutorialMap.obj", true);
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
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjMorphLoader>(entity).LoadMeshs("drone/shield", true);
		shieldDrone = entity;
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

void Tutorial::Update()
{
	/*if (ControllerInput::GetButtonDown(BUTTON::SELECT, CONUSER::ONE)) {
		if (ControllerInput::GetButton(BUTTON::RB, CONUSER::ONE)) {
			QueueSceneChange(2);
			return;
		}
		else {
			if (++m_camCount > 4)	m_camCount = 1;
			if (m_camCount == 2) {
				ECS::GetComponent<Camera>(cameraEnt[0]).ResizeWindow(width / 2.f, height);
				ECS::GetComponent<Camera>(cameraEnt[1]).ResizeWindow(width / 2.f, height);
				Player::SetUIAspect(width / 2.f, height);
			}
			else {
				ECS::GetComponent<Camera>(cameraEnt[0]).ResizeWindow(width, height);
				ECS::GetComponent<Camera>(cameraEnt[1]).ResizeWindow(width, height);
				Player::SetUIAspect(width, height);
			}
		}
	}*/

	if (Input::GetKeyDown(KEY::F)) {
		if (++m_camCount > 4)	m_camCount = 1;
		if (m_camCount == 2) {
			ECS::GetComponent<Camera>(cameraEnt[0]).ResizeWindow(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
			ECS::GetComponent<Camera>(cameraEnt[1]).ResizeWindow(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
			Player::SetUIAspect(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
		}
		else {
			ECS::GetComponent<Camera>(cameraEnt[0]).ResizeWindow(BackEnd::GetWidth(), BackEnd::GetHeight());
			ECS::GetComponent<Camera>(cameraEnt[1]).ResizeWindow(BackEnd::GetWidth(), BackEnd::GetHeight());
			Player::SetUIAspect(BackEnd::GetWidth(), BackEnd::GetHeight());
		}
	}

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

		Init(BackEnd::GetWidth(), BackEnd::GetHeight());
		QueueSceneChange(3);
	}

	ECS::GetComponent<Transform>(lightDrone).SetPosition(dronePath.Update(Time::dt).GetPosition()).SetRotation(dronePath.GetLookingForwards(0.5f));
	ECS::GetComponent<Transform>(cameraDrone).SetPosition(dronePath2.Update(Time::dt).GetPosition()).SetRotation(dronePath2.GetLookingForwards(0.5f));
	ECS::GetComponent<Transform>(speakerDrone).SetPosition(dronePath3.Update(Time::dt).GetPosition()).SetRotation(dronePath3.GetLookingForwards(0.5f));
	ECS::GetComponent<Transform>(shieldDrone).SetPosition(dronePath4.Update(Time::dt).GetPosition()).SetRotation(dronePath4.GetLookingForwards(0.5f));


	/*if (Input::GetKeyDown(KEY::FSLASH))	m_colliders.ToggleDraw();
	m_colliders.Update(Time::dt);
	if (Input::GetKeyDown(KEY::F10))	if (!m_colliders.SaveToFile(false))	std::cout << "file save failed\n";
	if (Input::GetKeyDown(KEY::F1))		if (!m_colliders.LoadFromFile())	std::cout << "file load failed\n";*/
}

Scene* Tutorial::Reattach() {
	ECS::AttachRegistry(&m_reg);
	if (m_world) {
		PhysBody::Init(m_world);
		ECS::AttachWorld(m_world);
		Rendering::hitboxes = &m_colliders;
	}

	m_frameEffects.Resize(BackEnd::GetWidth(), BackEnd::GetHeight());

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
			ECS::AttachComponent<Camera>(cameraEnt[i]).SetFovDegrees(60.f).ResizeWindow(BackEnd::GetHalfWidth(), BackEnd::GetHeight());
		else
			ECS::AttachComponent<Camera>(cameraEnt[i]).SetFovDegrees(60.f).ResizeWindow(BackEnd::GetWidth(), BackEnd::GetHeight());

		bodyEnt[i] = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(bodyEnt[i]).CreatePlayer(bodyEnt[i], BLM::GLMQuat, glm::vec3(0, 1.5f, 0));
		ECS::AttachComponent<Player>(bodyEnt[i]).Init(
			LeaderBoard::players[temp].user, LeaderBoard::players[temp].model, i).SetRotation(glm::radians(180.f), 0);

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
