#include "MapEditorScene.h"

void MapEditor::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "playtestmap";
	std::cout << "filename: " + input + "\n";
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";
	
	width = windowWidth;
	height = windowHeight;

	m_camCount = 1;

	/// Creating Entities
	cameraEnt = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt).SetFovDegrees(60.f).ResizeWindow(width, height);
	ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(0, 0, 0));

	rayPosEnt = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(rayPosEnt).LoadMesh("models/CameraDrone.obj", true);
	ECS::GetComponent<Transform>(rayPosEnt).SetPosition(glm::vec3(0, 0.f, 0)).SetScale(0.5f);
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/Cringe4.5.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -30.f, 0)).SetScale(4.f);
	}

	/// End of creating entities
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 0.5f);
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

	/*if (ControllerInput::GetButtonDown(BUTTON::SELECT, CONUSER::ONE)) {
		m_nextScene = 1;
		return;
	}*/

	if (Input::GetKeyDown(KEY::F)) {
		if (BackEnd::GetFullscreen())	BackEnd::SetTabbed(width, height);
		else							BackEnd::SetFullscreen();
	}

	//camera movement here
	{
		auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);
		glm::vec3 change = BLM::GLMzero;
		//moving
		if (Input::GetKey(KEY::W))		{ change.z -= speed * Time::dt; }
		if (Input::GetKey(KEY::S))		{ change.z += speed * Time::dt; }

		if (Input::GetKey(KEY::D))		{ change.x += speed * Time::dt; }
		if (Input::GetKey(KEY::A))		{ change.x -= speed * Time::dt; }

		if (Input::GetKey(KEY::SPACE))	{ change.y += speed * Time::dt; }
		if (Input::GetKey(KEY::LSHIFT) ||
			Input::GetKey(KEY::LCTRL))	{ change.y -= speed * Time::dt; }

		if (change != BLM::GLMzero) {
			change = glm::rotate(glm::angleAxis(rot.y, BLM::GLMup), change);
			camTrans.SetPosition(camTrans.GetLocalPosition() + change);
		}

		bool rotChanged = false;
		//rotation
		if (Input::GetKey(KEY::UP))		{ rot.x += rotSpeed * Time::dt; rotChanged = true; }
		if (Input::GetKey(KEY::DOWN))	{ rot.x -= rotSpeed * Time::dt; rotChanged = true; }
		if (Input::GetKey(KEY::LEFT))	{ rot.y += rotSpeed * Time::dt; rotChanged = true; }
		if (Input::GetKey(KEY::RIGHT))	{ rot.y -= rotSpeed * Time::dt; rotChanged = true; }

		if (rotChanged) {
			camTrans.SetRotation(glm::angleAxis(rot.y, BLM::GLMup) * glm::angleAxis(rot.x, glm::vec3(1, 0, 0)));
		}

		if (showRay) {
			btVector3 rayPos = PhysBody::GetRaycast(camTrans.GetGlobalPosition(), camTrans.GetForwards() * -10000.f);
			if (rayPos != btVector3()) {
				ECS::GetComponent<Transform>(rayPosEnt).SetPosition(rayPos);
			}
		}
		else {
			ECS::GetComponent<Transform>(rayPosEnt).SetPosition(glm::vec3(0, 1000, 0));
		}
	}

	/// End of loop
}

void MapEditor::Exit()
{
	if (saveOnExit) {
		if (m_colliders.SaveToFile())
			std::cout << (debugText = "file save success\n");
		else
			std::cout << (debugText = "file save failed\n");
	}
}

Scene* MapEditor::Reattach()
{
	return Scene::Reattach();
}

void MapEditor::ImGuiFunc()
{
	ImGui::SliderFloat("Movement speed", &speed, 0, 15);
	ImGui::SliderFloat("Camera rotation speed", &rotSpeed, 0, 5);

	ImGui::Checkbox("Save on exit", &saveOnExit);
	ImGui::Checkbox("Shoot ray from camera", &showRay);

	if (ImGui::Button("Toggle render hitboxes") || Input::GetKeyDown(KEY::FSLASH)) {
		debugText = m_colliders.ToggleDraw() ? "drawing hitboxes" : "not drawing hitboxes";
	}

	m_colliders.Update(Time::dt, cameraEnt);


	if (ImGui::Button("Overwrite save")) {
		if (m_colliders.SaveToFile()) {
			std::cout << (debugText = "manual file overwrite success\n");
		}
		else
			std::cout << (debugText = "manual file overwrite failed\n");
	}
	if (ImGui::Button("Save as new file")) {
		if (m_colliders.SaveToFile(false)) {
			std::cout << (debugText = "manual file save success\n");
		}
		else
			std::cout << (debugText = "manual file save failed\n");
	}
	if (ImGui::Button("Load last save")) {
		if (m_colliders.LoadFromFile()) {
			std::cout << (debugText = "manual file load success\n");
		}
		else
			std::cout << (debugText = "manual file load failed\n");
	}
	ImGui::Text(debugText.c_str());
}

