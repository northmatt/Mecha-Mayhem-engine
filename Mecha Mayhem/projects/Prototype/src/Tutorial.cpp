#include "Tutorial.h"
#include "LeaderBoard.h"

void Tutorial::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "tutorial";
	std::cout << "filename: " + input + "\n";
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";

	width = windowWidth;
	height = windowHeight;

	m_camCount = 4;

	/// Creating Entities
	cameraEnt4 = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt4).SetFovDegrees(60.f).ResizeWindow(width, height);

	cameraEnt3 = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt3).SetFovDegrees(60.f).ResizeWindow(width, height);

	cameraEnt2 = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt2).SetFovDegrees(60.f).ResizeWindow(width, height);

	cameraEnt1 = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt1).SetFovDegrees(60.f).ResizeWindow(width, height);


	bodyEnt1 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt1).CreatePlayer(bodyEnt1, startQuat, glm::vec3(0, 1.5f, 0));
	ECS::AttachComponent<Player>(bodyEnt1).Init(CONUSER::ONE, 1).SetRotation(glm::radians(180.f), 0);

	Head1 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head1).SetPosition(glm::vec3(0, 0.75f, 0)).
		ChildTo(bodyEnt1).SetUsingParentScale(false);
	ECS::GetComponent<Transform>(cameraEnt1).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head1).SetUsingParentScale(false);



	bodyEnt2 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt2).CreatePlayer(bodyEnt2, startQuat, glm::vec3(0, 1.5f, 0));
	ECS::AttachComponent<Player>(bodyEnt2).Init(CONUSER::TWO, 2).SetRotation(glm::radians(180.f), 0);

	Head2 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head2).SetPosition(glm::vec3(0, 0.75f, 0)).
		ChildTo(bodyEnt2).SetUsingParentScale(false);
	ECS::GetComponent<Transform>(cameraEnt2).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head2).SetUsingParentScale(false);


	bodyEnt3 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt3).CreatePlayer(bodyEnt3, startQuat, glm::vec3(0, 1.5f, 0));
	ECS::AttachComponent<Player>(bodyEnt3).Init(CONUSER::THREE, 3).SetRotation(glm::radians(180.f), 0);

	Head3 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head3).SetPosition(glm::vec3(0, 0.75f, 0)).
		ChildTo(bodyEnt3).SetUsingParentScale(false);
	ECS::GetComponent<Transform>(cameraEnt3).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head3).SetUsingParentScale(false);



	bodyEnt4 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt4).CreatePlayer(bodyEnt4, startQuat, glm::vec3(0, 1.5f, 0));
	ECS::AttachComponent<Player>(bodyEnt4).Init(CONUSER::FOUR, 4).SetRotation(glm::radians(180.f), 0);

	Head4 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head4).SetPosition(glm::vec3(0, 0.75f, 0)).
		ChildTo(bodyEnt4).SetUsingParentScale(false);
	ECS::GetComponent<Transform>(cameraEnt4).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head4).SetUsingParentScale(false);
	
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, startQuat, glm::vec3(0, 1, -12))
			.GetBody()->setMassProps(0, btVector3(0, -1, 0));
		ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 69).SetSpawn(glm::vec3(4, 1, -59)).SetMaxHealth(5);
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, startQuat, glm::vec3(0, 2.5f, -20))
			.GetBody()->setMassProps(0, btVector3(0, -1, 0));
		ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 69).SetSpawn(glm::vec3(-4, 1, -59)).SetMaxHealth(10);
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<Spawner>(entity).Init(0.3f, 2.5f);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 0, -18));
	}
	//dummies
	for (int i(0); i < 3; ++i) {
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, startQuat, glm::vec3(-7.5f, 1, i * 7.5f - 55))
				.GetBody()->setMassProps(0, btVector3(0, -1, 0));
			ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 69).SetSpawn(glm::vec3(-7.5f, 1, i * 7.5f - 55));
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<PhysBody>(entity).CreatePlayer(entity, startQuat, glm::vec3(7.5f, 1, i * 7.5f - 55))
				.GetBody()->setMassProps(0, btVector3(0, -1, 0));
			ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 69).SetSpawn(glm::vec3(7.5f, 1, i * 7.5f - 55));
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Spawner>(entity).Init(0.3f, 5.f);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(5.f, 0, i * 7.5f - 55));
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Spawner>(entity).Init(0.3f, 5.f);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(-5.f, 0, i * 7.5f - 55));
		}
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
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 1.f);
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
	if (ControllerInput::GetButtonDown(BUTTON::SELECT, CONUSER::ONE)) {
		if (ControllerInput::GetButton(BUTTON::RB, CONUSER::ONE)) {
			QueueSceneChange(2);
			return;
		}
		else {
			if (++m_camCount > 4)	m_camCount = 1;
			if (m_camCount == 2) {
				ECS::GetComponent<Camera>(cameraEnt1).ResizeWindow(width / 2.f, height);
				ECS::GetComponent<Camera>(cameraEnt2).ResizeWindow(width / 2.f, height);
				Player::SetUIAspect(width / 2.f, height);
			}
			else {
				ECS::GetComponent<Camera>(cameraEnt1).ResizeWindow(width, height);
				ECS::GetComponent<Camera>(cameraEnt2).ResizeWindow(width, height);
				Player::SetUIAspect(width, height);
			}
		}
	}

	if (ControllerInput::GetButtonDown(BUTTON::START, CONUSER::ONE)) {
		if (ControllerInput::GetButton(BUTTON::RB, CONUSER::ONE)) {
			if (BackEnd::GetFullscreen())	BackEnd::SetTabbed(width, height);
			else							BackEnd::SetFullscreen();
		}
	}

	for (size_t i(0); i < 4; ++i) {
		if (ControllerInput::GetButtonDown(BUTTON::START, CONUSER(i))) {
			m_paused = !m_paused;
			return;
		}
	}

	if (m_paused)
		return;


	auto &p1 = ECS::GetComponent<Player>(bodyEnt1);
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
	);

	if (p1.HasWon(killGoal) || p2.HasWon(killGoal) || p3.HasWon(killGoal) || p4.HasWon(killGoal)) {
		LeaderBoard::players[0].score = ECS::GetComponent<Player>(bodyEnt1).GetScore();
		LeaderBoard::players[1].score = ECS::GetComponent<Player>(bodyEnt2).GetScore();
		LeaderBoard::players[2].score = ECS::GetComponent<Player>(bodyEnt3).GetScore();
		LeaderBoard::players[3].score = ECS::GetComponent<Player>(bodyEnt4).GetScore();

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

		width = BackEnd::GetHalfWidth() * 2;
		height = BackEnd::GetHalfHeight() * 2;

		Init(width, height);
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

	m_frameEffects.Resize(BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);

	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;

	ECS::GetComponent<Player>(bodyEnt1).Init(LeaderBoard::players[0].user, LeaderBoard::players[0].model);
	ECS::GetComponent<Player>(bodyEnt2).Init(LeaderBoard::players[1].user, LeaderBoard::players[1].model);
	ECS::GetComponent<Player>(bodyEnt3).Init(LeaderBoard::players[2].user, LeaderBoard::players[2].model);
	ECS::GetComponent<Player>(bodyEnt4).Init(LeaderBoard::players[3].user, LeaderBoard::players[3].model);

	m_camCount = LeaderBoard::playerCount;

	if (m_camCount == 2) {
		ECS::GetComponent<Camera>(cameraEnt1).ResizeWindow(width / 2.f, height);
		ECS::GetComponent<Camera>(cameraEnt2).ResizeWindow(width / 2.f, height);
		Player::SetUIAspect(width / 2.f, height);
	}
	else {
		ECS::GetComponent<Camera>(cameraEnt1).ResizeWindow(width, height);
		ECS::GetComponent<Camera>(cameraEnt2).ResizeWindow(width, height);
		Player::SetUIAspect(width, height);
	}

	Player::SetCamDistance(camDistance);
	Player::SetSkyPos(glm::vec3(0, 50, -45));

	return this;
}
