#include "MainMenu.h"
#include "LeaderBoard.h"

void MainMenu::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);

	width = windowWidth;
	height = windowHeight;

	camera = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(camera).SetFovDegrees(60.f)
		.ResizeWindow(windowWidth, windowHeight).SetNear(0.3f);

	title = ECS::CreateEntity();
	ECS::AttachComponent<ObjMorphLoader>(title).LoadMeshs("title", true);
	ECS::GetComponent<Transform>(title).SetPosition(glm::vec3(0.f, 0.15f, -0.9f)).SetScale(0.25f).ChildTo(camera);

	text = ECS::CreateEntity();
	ECS::AttachComponent<Sprite>(text).Init("Start Text.png", -4.f, 1.f).SetScale(0.075f);
	ECS::GetComponent<Transform>(text).SetPosition(glm::vec3(0.f, -0.1f, -0.35f)).ChildTo(camera);
	
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/cringe.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -5, 0));
	}
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<Sprite>(entity).Init("CharSelect.png", -12.326f, 1.5f);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 103, -8));
	}

	for (int x(0); x < 4; ++x) {
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Sprite>(entity).Init("ArrowL.png", -0.75f, 0.75f);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(x * 4 - 7, 99, -8));
		}
		{
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Sprite>(entity).Init("ArrowR.png", -0.75f, 0.75f);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(x * 4 - 5, 99, -8));
		}
		models[x] = ECS::CreateEntity();
		ECS::AttachComponent<Player>(models[x]).Init(CONUSER::NONE, LeaderBoard::players[x].model);
		ECS::AttachComponent<PhysBody>(models[x]);
		ECS::GetComponent<Transform>(models[x]).SetPosition(glm::vec3(x * 4 - 6, 99, -8)).SetRotation(glm::angleAxis(BLM::pi, BLM::GLMup)).SetScale(1.5f);
	}

	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<Sprite>(entity).Init(glm::vec4(0.5f, 0.5f, 1.f, 1.f), -19, 10);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, 100, -10));
	}
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

	if (m_scenePos == 0) {
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

		Rendering::LightsPos[4] = Rendering::LightsPos[5] = ECS::GetComponent<Transform>(camera).SetPosition(
			cameraPath.Update(Time::dt).GetPosition()).SetRotation(
				cameraPath.GetLookingForwards(Time::dt) * glm::angleAxis(glm::radians(rx * 4), glm::vec3(0, -1, 0))
				* glm::angleAxis(glm::radians(ry * 4), glm::vec3(1, 0, 0))).GetGlobalPosition();


		if (m_exit) {
			cameraPath.SetSpeed(zoomTime += 8 * Time::dt);
			ECS::GetComponent<Transform>(text).SetScale(1.f / zoomTime);
			if (ECS::GetComponent<ObjMorphLoader>(title).IsDone()) {
				ECS::GetComponent<ObjMorphLoader>(title).ToggleDirection();
				ECS::GetComponent<Transform>(camera).SetPosition(glm::vec3(0, 100, 0)).SetRotation(BLM::GLMQuat);
				ECS::GetComponent<Transform>(text).SetScale(1.f);

				ECS::GetComponent<Transform>(title).SetRotation(BLM::GLMQuat).UnChild();
				ECS::GetComponent<Transform>(text).SetRotation(BLM::GLMQuat).UnChild();

				cameraPath.SetSpeed(1);
				m_exit = false;
				zoomTime = 1;
				m_scenePos = 1;
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

			//exit
			for (int x(0); x < 4; ++x) {
				if (ControllerInput::GetButton(BUTTON::B, CONUSER(x))) {
					if (ControllerInput::GetButtonDown(BUTTON::B, CONUSER(x))) {
						m_exitHoldTimer = 1.f;
					}
					m_exitHoldTimer -= Time::dt;
					if (m_exitHoldTimer <= 0.f) {
						m_exitGame = true;
					}
					ECS::GetComponent<Sprite>(text).SetWidth(-4.f * m_exitHoldTimer);
				}
				else if (ControllerInput::GetButtonUp(BUTTON::B, CONUSER(x))) {
					ECS::GetComponent<Sprite>(text).SetWidth(-4.f);
				}
			}
		}
	}
	else if (m_scenePos == 1) {
		//character select

		int lx(0);

		int allHolding = 0;
		int playerCount = 0;
		//exit
		for (int x(0); x < 4; ++x) {
			auto &p = ECS::GetComponent<Player>(models[x]);

			if (p.IsPlayer()) {
				if (ControllerInput::GetButtonDown(BUTTON::B, CONUSER(x))) {
					p.Init(LeaderBoard::players[x].user = CONUSER::NONE, 0);
					playerSwapped[x] = true;
					continue;
				}

				float rx = ControllerInput::GetRX(CONUSER(x));
				if (rx != 0) {
					glm::quat rot = ECS::GetComponent<Transform>(models[x]).GetLocalRotation();
					ECS::GetComponent<Transform>(models[x]).SetRotation(glm::rotate(rot, rx * Time::dt, BLM::GLMup));
				}
				lx = ControllerInput::GetLXRaw(CONUSER(x));

				if (lx < 0) {
					if (!playerSwapped[x]) {
						if (--(LeaderBoard::players[x].model) < 1)
							LeaderBoard::players[x].model = maxSelect;

						p.Init(CONUSER::FOUR, LeaderBoard::players[x].model);

						playerSwapped[x] = true;
					}
				}
				else if (lx > 0) {
					if (!playerSwapped[x]) {
						if (++(LeaderBoard::players[x].model) > maxSelect)
							LeaderBoard::players[x].model = 1;

						p.Init(CONUSER::FOUR, LeaderBoard::players[x].model);

						playerSwapped[x] = true;
					}
				}
				else {
					playerSwapped[x] = false;
				}

				++playerCount;
				allHolding += ControllerInput::GetButton(BUTTON::START, CONUSER(x));
			}
			else if (ControllerInput::GetButtonDown(BUTTON::A, CONUSER(x))) {
				if (LeaderBoard::players[x].model == 0)
					LeaderBoard::players[x].model = 1;
				LeaderBoard::players[x].user = CONUSER(x);
				p.Init(CONUSER::FOUR, LeaderBoard::players[x].model);
			}


			if (ControllerInput::GetButton(BUTTON::B, CONUSER(x))) {
				if (!playerSwapped[x]) {
					if (ControllerInput::GetButtonDown(BUTTON::B, CONUSER(x))) {
						m_exitHoldTimer = 1.f;
					}
					m_exitHoldTimer -= Time::dt;
					if (m_exitHoldTimer <= 0.f) {
						m_scenePos = 0;
						m_exitHoldTimer = 1.f;

						ECS::GetComponent<Transform>(title).SetRotation(BLM::GLMQuat).ChildTo(camera);
						ECS::GetComponent<Transform>(text).SetRotation(BLM::GLMQuat).ChildTo(camera);
					}
					//ECS::GetComponent<Sprite>(text).SetWidth(-4.f * m_exitHoldTimer);
				}
			}
			else if (ControllerInput::GetButtonUp(BUTTON::B, CONUSER(x))) {
				playerSwapped[x] = false;
				//ECS::GetComponent<Sprite>(text).SetWidth(-4.f);
			}
		}

		if (allHolding == playerCount && playerCount > 0) {
			m_confirmTimer -= Time::dt;
			if (m_confirmTimer <= 0) {
				m_nextScene = 1;
				m_scenePos = 0;
				m_exitHoldTimer = 1.f;

				ECS::GetComponent<Transform>(title).SetRotation(BLM::GLMQuat).ChildTo(camera);
				ECS::GetComponent<Transform>(text).SetRotation(BLM::GLMQuat).ChildTo(camera);

				LeaderBoard::playerCount = playerCount;
			}
		}
		else {
			m_confirmTimer = 1.f;
		}
	}
}

void MainMenu::Exit()
{
}
