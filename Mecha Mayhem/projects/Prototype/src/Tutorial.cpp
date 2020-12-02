#include "Tutorial.h"

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
	ECS::AttachComponent<Player>(bodyEnt2).Init(CONUSER::TWO, 1).SetRotation(glm::radians(180.f), 0);

	Head2 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head2).SetPosition(glm::vec3(0, 0.75f, 0)).
		ChildTo(bodyEnt2).SetUsingParentScale(false);
	ECS::GetComponent<Transform>(cameraEnt2).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head2).SetUsingParentScale(false);


	bodyEnt3 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt3).CreatePlayer(bodyEnt3, startQuat, glm::vec3(0, 1.5f, 0));
	ECS::AttachComponent<Player>(bodyEnt3).Init(CONUSER::THREE, 1).SetRotation(glm::radians(180.f), 0);

	Head3 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head3).SetPosition(glm::vec3(0, 0.75f, 0)).
		ChildTo(bodyEnt3).SetUsingParentScale(false);
	ECS::GetComponent<Transform>(cameraEnt3).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head3).SetUsingParentScale(false);



	bodyEnt4 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt4).CreatePlayer(bodyEnt4, startQuat, glm::vec3(0, 1.5f, 0));
	ECS::AttachComponent<Player>(bodyEnt4).Init(CONUSER::FOUR, 1).SetRotation(glm::radians(180.f), 0);

	Head4 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head4).SetPosition(glm::vec3(0, 0.75f, 0)).
		ChildTo(bodyEnt4).SetUsingParentScale(false);
	ECS::GetComponent<Transform>(cameraEnt4).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head4).SetUsingParentScale(false);
	
	
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(entity).Init(entity, 0.5f, 2, glm::vec3(0, 1.5f, 0), 1, true).
			SetPosition(btVector3(-7.5f, 5, -50)).SetRotation(startQuat).GetBody()->setAngularFactor(BLM::BTzero);
		ECS::AttachComponent<Player>(entity).Init(CONUSER::NONE, 1).SetSpawn(glm::vec3(-7.5f, 5, -50));
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("maps/tutorialMap.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 0, 0));
	}

	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjMorphLoader>(entity).LoadMeshs("title", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 2.5f, -50)).SetScale(2.f);
	}

	/// End of creating entities
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 1.f);
	Rendering::hitboxes = &m_colliders;
	Rendering::effects = &m_effects;

	Player::SetUIAspect(width, height);
	Player::SetCamDistance(camDistance);

	Player::SetSkyPos(glm::vec3(0, 50, -45));
}

void Tutorial::Update()
{
	if (ControllerInput::GetButtonDown(BUTTON::SELECT, CONUSER::ONE)) {
		m_nextScene = 2;
	}
	if (ControllerInput::GetButtonDown(BUTTON::START, CONUSER::ONE)) {
		if (ControllerInput::GetButton(BUTTON::RB, CONUSER::ONE)) {
			if (BackEnd::GetFullscreen())	BackEnd::SetTabbed(width, height);
			else							BackEnd::SetFullscreen();
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

	ECS::GetComponent<Player>(bodyEnt1).GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt1),
		ECS::GetComponent<Transform>(Head1),
		ECS::GetComponent<Transform>(cameraEnt1)
	);

	ECS::GetComponent<Player>(bodyEnt2).GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt2),
		ECS::GetComponent<Transform>(Head2),
		ECS::GetComponent<Transform>(cameraEnt2)
	);

	ECS::GetComponent<Player>(bodyEnt3).GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt3),
		ECS::GetComponent<Transform>(Head3),
		ECS::GetComponent<Transform>(cameraEnt3)
	);

	ECS::GetComponent<Player>(bodyEnt4).GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt4),
		ECS::GetComponent<Transform>(Head4),
		ECS::GetComponent<Transform>(cameraEnt4)
	);

	/*if (Input::GetKeyDown(KEY::FSLASH))	m_colliders.ToggleDraw();
	m_colliders.Update(Time::dt);
	if (Input::GetKeyDown(KEY::F10))	if (!m_colliders.SaveToFile(false))	std::cout << "file save failed\n";
	if (Input::GetKeyDown(KEY::F1))		if (!m_colliders.LoadFromFile())	std::cout << "file load failed\n";*/
}
