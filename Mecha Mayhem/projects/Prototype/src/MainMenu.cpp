#include "MainMenu.h"

void MainMenu::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);

	camera = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(camera).SetFovDegrees(60.f)
		.ResizeWindow(windowWidth, windowHeight).SetNear(0.3f);

	title = ECS::CreateEntity();
	ECS::AttachComponent<ObjMorphLoader>(title).LoadMeshs("title", true);
	ECS::GetComponent<Transform>(title).SetPosition(glm::vec3(0.f, 0.15f, -0.9f)).SetScale(0.25f).ChildTo(camera);

	text = ECS::CreateEntity();
	ECS::AttachComponent<Sprite>(text).Init("Start Text.png", -4, 1).SetScale(0.075f);
	ECS::GetComponent<Transform>(text).SetPosition(glm::vec3(0.f, -0.1f, -0.35f)).ChildTo(camera);
	
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/cringe.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -5, 0));
	}
}

void MainMenu::Update()
{
	float lx = ControllerInput::GetLX(CONUSER::ONE) + ControllerInput::GetLX(CONUSER::TWO) +
		ControllerInput::GetLX(CONUSER::THREE) + ControllerInput::GetLX(CONUSER::FOUR);
	float ly = ControllerInput::GetLY(CONUSER::ONE) + ControllerInput::GetLY(CONUSER::TWO) +
		ControllerInput::GetLY(CONUSER::THREE) + ControllerInput::GetLY(CONUSER::FOUR);

	float rx = ControllerInput::GetRX(CONUSER::ONE) + ControllerInput::GetRX(CONUSER::TWO) +
		ControllerInput::GetRX(CONUSER::THREE) + ControllerInput::GetRX(CONUSER::FOUR);
	float ry = ControllerInput::GetRY(CONUSER::ONE) + ControllerInput::GetRY(CONUSER::TWO) +
		ControllerInput::GetRY(CONUSER::THREE) + ControllerInput::GetRY(CONUSER::FOUR);


	Rendering::LightsPos[2] = ECS::GetComponent<Transform>(title).SetRotation(
		glm::angleAxis(glm::radians(lx * 6), BLM::GLMup) * glm::angleAxis(glm::radians(ly * 6), glm::vec3(-1, 0, 0))
	).GetGlobalPosition();

	Rendering::LightsPos[3] = ECS::GetComponent<Transform>(text).SetRotation(
		glm::angleAxis(glm::radians(lx * 3), BLM::GLMup) * glm::angleAxis(glm::radians(ly * 3), glm::vec3(-1, 0, 0))
	).GetGlobalPosition();

	Rendering::LightsPos[4] = ECS::GetComponent<Transform>(camera).SetPosition(
		cameraPath.Update(Time::dt).GetPosition()).SetRotation(
			cameraPath.GetLookingForwards(Time::dt) * glm::angleAxis(glm::radians(rx * 4), glm::vec3(0, -1, 0))
			* glm::angleAxis(glm::radians(ry * 4), glm::vec3(1, 0, 0))).GetGlobalPosition();


	if (m_exit) {
		cameraPath.SetSpeed(zoomTime += 8 * Time::dt);
		ECS::GetComponent<Transform>(text).SetScale(1.f / zoomTime);
		if (ECS::GetComponent<ObjMorphLoader>(title).IsDone()) {
			ECS::GetComponent<ObjMorphLoader>(title).ToggleDirection();
			ECS::GetComponent<Transform>(text).SetScale(1.f);
			cameraPath.SetSpeed(1);
			m_exit = false;
			zoomTime = 1;
			m_nextScene = 1;
		}
	}
	else if (ECS::GetComponent<ObjMorphLoader>(title).IsDone()) {
		cameraPath.SetSpeed((fabsf(rx) + fabsf(ry) + fabsf(lx) + fabsf(ly)) / 4.f + 1);
		if (ControllerInput::GetButtonDown(BUTTON::A, CONUSER::ONE) ||
			ControllerInput::GetButtonDown(BUTTON::A, CONUSER::TWO) ||
			ControllerInput::GetButtonDown(BUTTON::A, CONUSER::THREE) ||
			ControllerInput::GetButtonDown(BUTTON::A, CONUSER::FOUR)) {
			ECS::GetComponent<ObjMorphLoader>(title).ToggleDirection();
			m_exit = true;
		}
	}

}

void MainMenu::Exit()
{
}
