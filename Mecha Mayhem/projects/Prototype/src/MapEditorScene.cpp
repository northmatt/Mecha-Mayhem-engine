#include "MapEditorScene.h"

void MapEditor::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "testmap";
	std::cout << "filename: " + input + "\n";
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";
	
	width = windowWidth;
	height = windowHeight;

	m_camCount = 1;

	/// Creating Entities
	cameraEnt1 = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt1).SetFovDegrees(60.f).ResizeWindow(width, height);
	ECS::GetComponent<Transform>(cameraEnt1).SetPosition(glm::vec3(0, 0, 0));


	bodyEnt1 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt1).CreatePlayer(bodyEnt1, BLM::GLMQuat, glm::vec3(0, 1.5f, 0));

	/// End of creating entities
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 1.f);
	Rendering::hitboxes = &m_colliders;
	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;

	m_frameEffects.Init(width, height);

	Player::SetUIAspect(width, height);
	Player::SetSkyPos(glm::vec3(0, 50, 0));
}

void MapEditor::Update()
{
	/// start of loop

	if (ControllerInput::GetButtonDown(BUTTON::SELECT, CONUSER::ONE)) {
		m_nextScene = 0;
		return;
	}

	if (ControllerInput::GetButtonDown(BUTTON::START, CONUSER::ONE)) {
		if (ControllerInput::GetButton(BUTTON::RB, CONUSER::ONE)) {
			if (BackEnd::GetFullscreen())	BackEnd::SetTabbed(width, height);
			else							BackEnd::SetFullscreen();
		}
	}

	//camera movement here

	/// End of loop
	/*if (Input::GetKeyDown(KEY::FSLASH))	m_colliders.ToggleDraw();
	m_colliders.Update(Time::dt);
	if (Input::GetKeyDown(KEY::F10))	if (!m_colliders.SaveToFile(false))	std::cout << "file save failed\n";
	if (Input::GetKeyDown(KEY::F1))		if (!m_colliders.LoadFromFile())	std::cout << "file load failed\n";*/
}

void MapEditor::Exit()
{
	if (saveOnExit)
		if (!m_colliders.SaveToFile())
			std::cout << "file save failed\n";
}

