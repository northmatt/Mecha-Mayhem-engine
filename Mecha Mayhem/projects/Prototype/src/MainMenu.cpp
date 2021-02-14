#include "MainMenu.h"
#include "LeaderBoard.h"

void MainMenu::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);

	width = windowWidth;
	height = windowHeight;

	int Awidth, Aheight;
	glfwGetWindowSize(BackEnd::GetWindow(), &Awidth, &Aheight);

	{
		auto bloomEffectObject = ECS::CreateEntity();
		ECS::AttachComponent<BloomEffect>(bloomEffectObject);
		simpleBEffect = &ECS::GetComponent<BloomEffect>(bloomEffectObject);
		simpleBEffect->Init(Awidth, Aheight);
	}

	{
		LUT3D cube("LUT/WarmLut.cube");

		auto colEffectObject = ECS::CreateEntity();
		ECS::AttachComponent<ColCor>(colEffectObject);
		simpleCCEffect = &ECS::GetComponent<ColCor>(colEffectObject);
		simpleCCEffect->Init(Awidth, Aheight, cube);
	}
	 
	{
		auto postEffectObject = ECS::CreateEntity();
		ECS::AttachComponent<PostEffect>(postEffectObject);
		simplePPEffect = &ECS::GetComponent<PostEffect>(postEffectObject);
		simplePPEffect->Init(Awidth, Aheight);
	}

	camera = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(camera).SetFovDegrees(60.f).ResizeWindow(windowWidth, windowHeight).SetNear(0.3f);
	 
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/Terrain.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -5, 0));
	}
	 
	{
		ship = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(ship).LoadMesh("models/Ship.obj", true);
		ECS::GetComponent<Transform>(ship).SetPosition(glm::vec3(0.f, -.5f, -2.f)).ChildTo(camera);
	}

	backGround = ECS::CreateEntity();
	ECS::AttachComponent<Sprite>(backGround).Init(glm::vec4(0.5f, 0.5f, 1.f, 1.f), -19, 10);
	ECS::GetComponent<Transform>(backGround).SetPosition(glm::vec3(0, 100, -10));
}

void MainMenu::Update()
{
	m_bg.loop();

	if (ControllerInput::GetButtonDown(BUTTON::START, CONUSER::ONE)) {
		if (ControllerInput::GetButton(BUTTON::RB, CONUSER::ONE)) {
			if (BackEnd::GetFullscreen())	BackEnd::SetTabbed(width, height);
			else							BackEnd::SetFullscreen();
		}
	}

	float lx = ControllerInput::GetLX(CONUSER::ONE) + ControllerInput::GetLX(CONUSER::TWO) +
		ControllerInput::GetLX(CONUSER::THREE) + ControllerInput::GetLX(CONUSER::FOUR);
	float ly = ControllerInput::GetLY(CONUSER::ONE) + ControllerInput::GetLY(CONUSER::TWO) +
		ControllerInput::GetLY(CONUSER::THREE) + ControllerInput::GetLY(CONUSER::FOUR);

	float rx = ControllerInput::GetRX(CONUSER::ONE) + ControllerInput::GetRX(CONUSER::TWO) +
		ControllerInput::GetRX(CONUSER::THREE) + ControllerInput::GetRX(CONUSER::FOUR);
	float ry = ControllerInput::GetRY(CONUSER::ONE) + ControllerInput::GetRY(CONUSER::TWO) +
		ControllerInput::GetRY(CONUSER::THREE) + ControllerInput::GetRY(CONUSER::FOUR);
		
	cameraPath.Update(Time::dt);

	Rendering::LightsPos[4] = ECS::GetComponent<Transform>(camera).SetPosition(cameraPath.GetPosition()).SetRotation(
		cameraPath.GetLookingForwards(0.01f)
		* glm::angleAxis(glm::radians(rx * 4), glm::vec3(0, -1, 0)) * glm::angleAxis(glm::radians(ry * 4), glm::vec3(1, 0, 0))
	).GetGlobalPosition();

	if (m_exit) {
		cameraPath.SetSpeed(zoomTime += 8 * Time::dt);
		//ECS::GetComponent<Transform>(text).SetScale(1.f / zoomTime);
		if (false) {
			ECS::GetComponent<Transform>(camera).SetPosition(glm::vec3(0, 100, 0)).SetRotation(BLM::GLMQuat);

			cameraPath.SetSpeed(1);
			m_exit = false;
			zoomTime = 1;
		}
	}
	else {
		cameraPath.SetSpeed((fabsf(rx) + fabsf(ry) + fabsf(lx) + fabsf(ly)) / 4.f + 1);
		if (ControllerInput::GetButtonDown(BUTTON::A, CONUSER::ONE) ||
			ControllerInput::GetButtonDown(BUTTON::A, CONUSER::TWO) ||
			ControllerInput::GetButtonDown(BUTTON::A, CONUSER::THREE) ||
			ControllerInput::GetButtonDown(BUTTON::A, CONUSER::FOUR)) {
//			ECS::GetComponent<ObjMorphLoader>(title).ToggleDirection();
			m_exit = true;
		}

		//exit
		for (int x(0); x < 4; ++x) {
			if (ControllerInput::GetButton(BUTTON::B, CONUSER(x))) {
				if (ControllerInput::GetButtonDown(BUTTON::B, CONUSER(x))) {
					m_exitHoldTimer = 1.f;
				}
				m_exitHoldTimer -= Time::dt;
				if (m_exitHoldTimer <= 0.f) {
					m_exitGame = true;
					break;
				}
				//ECS::GetComponent<Sprite>(text).SetWidth(-4.f * m_exitHoldTimer);
			}
		}
	}
}

void MainMenu::Exit()
{
}

void MainMenu::BackEndUpdate() {
	if (!m_paused) {
		if (m_world != nullptr) {

			m_world->stepSimulation(Time::dt, 10);

			m_reg.view<PhysBody, Transform>().each([](PhysBody& phys, Transform& trans) {
				if (phys.IsDynamic() || phys.Changed())
					trans.SetTransform(phys.GetTransform());
				}
			);
		}
		m_effects.Update();
	}

	simplePPEffect->Clear();

	//always render
	if (m_camCount < 1)
		m_camCount = 1;
	else if (m_camCount > 4)
		m_camCount = 4;
	Rendering::Update(&m_reg, m_camCount, m_paused, simplePPEffect);

	if (m_paused)   if (m_pauseSprite.IsValid()) {
		Rendering::DrawPauseScreen(m_pauseSprite);
	}
	
	simplePPEffect->UnbindBuffer();

	if (doCol) {
		simpleCCEffect->ApplyAffects(simplePPEffect);

		simplePPEffect->Clear();
		simplePPEffect->ApplyAffects(simpleCCEffect);
	}

	if (doBloom) {
		simpleBEffect->ApplyAffects(simplePPEffect);

		simplePPEffect->Clear();
		simplePPEffect->ApplyAffects(simpleBEffect);
	}

	simplePPEffect->DrawToScreen();
}