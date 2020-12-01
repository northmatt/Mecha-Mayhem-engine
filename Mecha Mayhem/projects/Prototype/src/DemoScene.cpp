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


	/// Creating Entitiesssss
	for (int x(0); x < 2; ++x)
	{
		auto entity = ECS::CreateEntity();
		auto& camCam2 = ECS::AttachComponent<Camera>(entity);
		camCam2.SetFovDegrees(60.f).ResizeWindow(width, height);
		glm::vec3 axis = glm::vec3(rand() % 2, rand() % 2, rand() % 2 + 1);
		ECS::GetComponent<Transform>(entity).
			SetPosition(glm::vec3(rand() % 21 - 10, 0, rand() % 21 - 10)).
			SetRotation(glm::rotate(glm::quat(1, 0, 0, 0), float(rand() % 360), axis));
	}
	cameraEnt2 = ECS::CreateEntity();
	auto& camCam2 = ECS::AttachComponent<Camera>(cameraEnt2);
	camCam2.SetFovDegrees(60.f).ResizeWindow(width, height);

	cameraEnt = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
	camCam.SetFovDegrees(60.f).ResizeWindow(width, height);

	bodyEnt = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt).Init(bodyEnt, 0.5f, 2, glm::vec3(0, 10, 0), 1, true).
		/*SetGravity(glm::vec3(0)).*/GetBody()->setAngularFactor(btVector3(0, 0, 0));
	ECS::GetComponent<PhysBody>(bodyEnt).GetBody()->setFriction(0);
	ECS::AttachComponent<Player>(bodyEnt).Init(CONUSER::ONE, 1);

	bodyEnt2 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt2).Init(bodyEnt2, 0.5f, 2, glm::vec3(0, 10, 0), 1, true).
		/*SetGravity(glm::vec3(0)).*/GetBody()->setAngularFactor(btVector3(0, 0, 0));
	ECS::GetComponent<PhysBody>(bodyEnt2).GetBody()->setFriction(0);
	ECS::AttachComponent<Player>(bodyEnt2).Init(CONUSER::TWO, 1);

	drone = ECS::CreateEntity();
	ECS::AttachComponent<ObjMorphLoader>(drone).LoadMeshs("sword").LoadMeshs("shield").
		LoadMeshs("drone").LoadMeshs("othershield/anim", true).LoadMeshs("anotherOne");
	ECS::GetComponent<Transform>(drone).ChildTo(bodyEnt).SetPosition(glm::vec3(0, 0, 1.f)).SetScale(0.3f);

	P = ECS::CreateEntity();
	ECS::GetComponent<Transform>(P).SetPosition(glm::vec3(0, 0.75f, 0)).ChildTo(bodyEnt);
	ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(P).SetUsingParentScale(false);

	P2 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(P2).SetPosition(glm::vec3(0, 0.75f, 0)).ChildTo(bodyEnt2);
	ECS::GetComponent<Transform>(cameraEnt2).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(P2).SetUsingParentScale(false);


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

	//cube
	epic = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(epic).LoadMesh("models/GodHimself.obj");

	Player::SetUIAspect(width, height);
}

void DemoScene::Update()
{
	//auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);
	auto& ballPhys = ECS::GetComponent<PhysBody>(bodyEnt).SetAwake();
	auto& ballPhys2 = ECS::GetComponent<PhysBody>(bodyEnt2).SetAwake();
	/// start of loop

	if (Input::GetKey(KEY::APOSTROPHE)) {
		ECS::GetComponent<Transform>(drone).SetRotation(glm::rotate(ECS::GetComponent<Transform>(drone).
			GetLocalRotation(), glm::radians(45 * Time::dt), glm::vec3(0, 1, 0))).SetPosition(
				glm::vec3(0, 0, 1.f) * glm::inverse(ECS::GetComponent<Transform>(drone).GetLocalRotation())
			);
	}
	if (Input::GetKey(KEY::SEMICOLON)) {
		ECS::GetComponent<Transform>(drone).SetRotation(glm::rotate(ECS::GetComponent<Transform>(drone).
			GetLocalRotation(), glm::radians(45 * Time::dt), glm::vec3(0, -1, 0))).SetPosition(
				glm::vec3(0, 0, 1.f) * glm::inverse(ECS::GetComponent<Transform>(drone).GetLocalRotation())
			);
	}

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

	if (Input::GetKeyDown(KEY::ESC)) {
		if (Input::GetKey(KEY::LSHIFT)) {
			if (screen = !screen)	BackEnd::SetTabbed(width, height);
			else					BackEnd::SetFullscreen();
		}
		else {
			if (++m_camCount > 4)	m_camCount = 1;
			if (m_camCount == 2) {
				ECS::GetComponent<Camera>(cameraEnt).ResizeWindow(width / 2, height);
				ECS::GetComponent<Camera>(cameraEnt2).ResizeWindow(width / 2, height);
				Player::SetUIAspect(width / 2.f, height);
			}
			else {
				ECS::GetComponent<Camera>(cameraEnt).ResizeWindow(width, height);
				ECS::GetComponent<Camera>(cameraEnt2).ResizeWindow(width, height);
				Player::SetUIAspect(width, height);
			}
		}
	}

	if (Input::GetKey(KEY::P)) {
		if (Input::GetKey(KEY::LSHIFT))
			Rendering::LightsColour[0] = glm::vec3(3.f);
		else
			Rendering::LightsColour[0] = glm::vec3((rand() % 30) / 10.f, (rand() % 30) / 10.f, (rand() % 30) / 10.f);
	}

	ECS::GetComponent<Player>(bodyEnt).GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt),
		ECS::GetComponent<Transform>(P),
		ECS::GetComponent<Transform>(cameraEnt)
	);

	ECS::GetComponent<Player>(bodyEnt2).GetInput(
		ECS::GetComponent<PhysBody>(bodyEnt2),
		ECS::GetComponent<Transform>(P2),
		ECS::GetComponent<Transform>(cameraEnt2)
	);

	/// End of loop
	if (Input::GetKeyDown(KEY::FSLASH))	m_colliders.ToggleDraw();
	m_colliders.Update(Time::dt);
	if (Input::GetKeyDown(KEY::F10))	if (!m_colliders.SaveToFile(false))	std::cout << "file save failed\n";
	if (Input::GetKeyDown(KEY::F1))		if (!m_colliders.LoadFromFile())	std::cout << "file load failed\n";
}

void DemoScene::Exit()
{
	if (!m_colliders.SaveToFile())
		std::cout << "file save failed\n";
}

