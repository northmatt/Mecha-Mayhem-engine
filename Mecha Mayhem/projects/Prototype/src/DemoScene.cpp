#include "DemoScene.h"

void DemoScene::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "testmap";
	std::cout << "filename: " + input + "\n";
	//std::cin >> input;
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


	drone = ECS::CreateEntity();
	ECS::AttachComponent<ObjMorphLoader>(drone).LoadMeshs("sword").LoadMeshs("shield").
		LoadMeshs("drone").LoadMeshs("othershield/anim", true).LoadMeshs("anotherOne");
	ECS::GetComponent<Transform>(drone).ChildTo(bodyEnt1).SetPosition(glm::vec3(0, 0, 1.f)).SetScale(0.3f);


	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/cringe.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -30, 0)).SetScale(4.f);
	}

	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjMorphLoader>(entity).LoadMeshs("spawner/spawner", true).SetBounce(true);
		ECS::GetComponent<Transform>(entity).SetPosition(
			glm::vec3(20.4755f, -6.89454f + 8.85118f/2.f, 21.7629f)
		).SetScale(1.25f);
	}

	/// End of creating entities
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 1.f);
	Rendering::hitboxes = &m_colliders;
	Rendering::effects = &m_effects;

	Player::SetUIAspect(width, height);
	Player::SetCamDistance(camDistance);
	Player::SetSkyPos(glm::vec3(0, 50, 0));
}

void DemoScene::Update()
{
	/// start of loop

	if (ControllerInput::GetButtonDown(BUTTON::SELECT, CONUSER::ONE)) {
		m_nextScene = 0;
	}

	ECS::GetComponent<Transform>(drone).SetPosition(dronePath.Update(Time::dt).GetPosition()).
		SetRotation(dronePath.GetLookingForwards(Time::dt));

	if (Input::GetKey(KEY::SEVEN))
		dronePath.SetSpeed(dronePath.GetSpeed() + Time::dt);
	else	dronePath.SetSpeed(1);

	if (Input::GetKeyDown(KEY::ONE)) {
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("sword");
	}
	if (Input::GetKeyDown(KEY::TWO)) {
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("shield");
	}
	if (Input::GetKeyDown(KEY::THREE)) {
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("drone");
	}
	if (Input::GetKeyDown(KEY::FOUR)) {
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("othershield/anim", true);
	}
	if (Input::GetKeyDown(KEY::FIVE)) {
		ECS::GetComponent<ObjMorphLoader>(drone).ToggleDirection();
	}
	if (Input::GetKeyDown(KEY::SIX)) {
		//ECS::GetComponent<ObjMorphLoader>(drone).ToggleBounce();
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("anotherOne");
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

	/// End of loop
	/*if (Input::GetKeyDown(KEY::FSLASH))	m_colliders.ToggleDraw();
	m_colliders.Update(Time::dt);
	if (Input::GetKeyDown(KEY::F10))	if (!m_colliders.SaveToFile(false))	std::cout << "file save failed\n";
	if (Input::GetKeyDown(KEY::F1))		if (!m_colliders.LoadFromFile())	std::cout << "file load failed\n";*/
}

void DemoScene::Exit()
{
	/*if (!m_colliders.SaveToFile())
		std::cout << "file save failed\n";*/
}

