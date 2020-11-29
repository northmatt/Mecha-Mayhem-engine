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


	/// Creating Entities
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

	cameraEnt1 = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt1);
	camCam.SetFovDegrees(60.f).ResizeWindow(width, height);

	bodyEnt1 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt1).Init(0.5f, 2, glm::vec3(0, 1.5f, 0), 1, true).
		/*SetGravity(glm::vec3(0)).*/
		SetRotation(startQuat).GetBody()->setAngularFactor(btVector3(0, 0, 0));
	ECS::GetComponent<PhysBody>(bodyEnt1).GetBody()->setFriction(0);
	ECS::AttachComponent<Player>(bodyEnt1).Init(CONUSER::ONE, 1).SetRotation(glm::radians(180.f), 0);

	bodyEnt2 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt2).Init(0.5f, 2, glm::vec3(0, 1.5f, 0), 1, true).
		/*SetGravity(glm::vec3(0)).*/
		SetRotation(startQuat).GetBody()->setAngularFactor(btVector3(0, 0, 0));
	ECS::GetComponent<PhysBody>(bodyEnt2).GetBody()->setFriction(0);
	ECS::AttachComponent<Player>(bodyEnt2).Init(CONUSER::TWO, 1).SetRotation(glm::radians(180.f), 0);

	auto Dio = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio).LoadMesh("models/Pistol.obj", true);

	auto Dio2 = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio2).LoadMesh("models/Pistol.obj", true);

	Head1 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head1).SetPosition(glm::vec3(0, 0.75f, 0)).ChildTo(bodyEnt1);
	ECS::GetComponent<Transform>(Dio).SetPosition(glm::vec3(0.4f, 0.25f, -0.5f)).
		ChildTo(bodyEnt1).SetUsingParentScale(false).SetScale(0.05f);
	ECS::GetComponent<Transform>(cameraEnt1).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head1).SetUsingParentScale(false);

	Head2 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(Head2).SetPosition(glm::vec3(0, 0.75f, 0)).ChildTo(bodyEnt2);
	ECS::GetComponent<Transform>(Dio2).SetPosition(glm::vec3(0.4f, 0.25f, -0.5f)).
		ChildTo(bodyEnt2).SetUsingParentScale(false).SetScale(0.05f);
	ECS::GetComponent<Transform>(cameraEnt2).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(Head2).SetUsingParentScale(false);
	
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("maps/tutorialMap.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 0, 0));
	}

	/// End of creating entities
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 1.f);
	Rendering::hitboxes = &m_colliders;

	Player::SetUIAspect(width, height);
	Player::SetCamDistance(camDistance);
}

void Tutorial::Update()
{
	if (Input::GetKeyDown(KEY::ESC)) {
		if (Input::GetKey(KEY::LSHIFT)) {
			if (screen = !screen)	BackEnd::SetTabbed(width, height);
			else					BackEnd::SetFullscreen();
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

	if (Input::GetKeyDown(KEY::FSLASH))	m_colliders.ToggleDraw();
	m_colliders.Update(Time::dt);
	if (Input::GetKeyDown(KEY::F10))	if (!m_colliders.SaveToFile(false))	std::cout << "file save failed\n";
	if (Input::GetKeyDown(KEY::F1))		if (!m_colliders.LoadFromFile())	std::cout << "file load failed\n";

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
}

void Tutorial::Exit()
{
	if (!m_colliders.SaveToFile())
		std::cout << "file save failed\n";
}
