#include "LeaderBoard.h"

void LeaderBoard::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);

	camera = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(camera).SetFovDegrees(60.f)
		.ResizeWindow(windowWidth, windowHeight).SetNear(0.3f);

	for (int i(0); i < 4; ++i) {
		//start parents off offscreen
		playerScores[i].parent = ECS::CreateEntity();
		ECS::GetComponent<Transform>(playerScores[i].parent).SetPosition(glm::vec3(0, 100, 0));


		//number is here because yea
		playerScores[i].number = ECS::CreateEntity();
		ECS::AttachComponent<Sprite>(playerScores[i].number);
		ECS::GetComponent<Transform>(playerScores[i].number).SetPosition(glm::vec3(-0.75f, 0, 0)).ChildTo(playerScores[i].parent);


		//create digits and parent with the right position
		playerScores[i].digit2 = ECS::CreateEntity();
		ECS::AttachComponent<Sprite>(playerScores[i].digit2);
		ECS::GetComponent<Transform>(playerScores[i].digit2).SetPosition(glm::vec3(0.75f, 0, 0)).ChildTo(playerScores[i].parent);

		playerScores[i].digit1 = ECS::CreateEntity();
		ECS::AttachComponent<Sprite>(playerScores[i].digit1);					//0.8f unit to the right of digit2
		ECS::GetComponent<Transform>(playerScores[i].digit1).SetPosition(glm::vec3(1.55f, 0, 0)).ChildTo(playerScores[i].parent);


		{	//P is a constant
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Sprite>(entity).Init("P.png", -1.81f, 1.f);
			ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(-1.025f, 0, -0.1f)).ChildTo(playerScores[i].parent);
		}

		playerScores[i].bar = ECS::CreateEntity();
		ECS::AttachComponent<Sprite>(playerScores[i].bar).Init("leaderboardbar.png", -3.88f, 0.2f).SetWidth(0);
		ECS::GetComponent<Transform>(playerScores[i].bar).SetPosition(glm::vec3(0, -0.6f, -0.2f)).ChildTo(playerScores[i].parent);

		SetDigits(0, i);
	}

	//title
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<Sprite>(entity).Init("leaderboard.png", -3.27f, 0.5f);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0.f, 1.2f, -3.f));
	}

	//continue text
	text = ECS::CreateEntity();
	ECS::AttachComponent<Sprite>(text).Init("Start Text.png", -4.f, 1.f).SetWidth(0);
	ECS::GetComponent<Transform>(text).SetPosition(glm::vec3(0.f, -3.25f, -8.f));

	{	//background
		auto backGround = ECS::CreateEntity();
		//ECS::AttachComponent<Sprite>(backGround).Init(glm::vec4(0.5f, 0.5f, 1.f, 1.f), -19, 10);
		ECS::AttachComponent<Sprite>(backGround).Init("genericbg.png", -19, 10);
		ECS::GetComponent<Transform>(backGround).SetPosition(glm::vec3(0, 0, -10));
	}

	playerEnt = ECS::CreateEntity();
	ECS::AttachComponent<Player>(playerEnt).Init(CONUSER::NONE, 0);
	ECS::AttachComponent<PhysBody>(playerEnt);
	ECS::GetComponent<Transform>(playerEnt).SetPosition(glm::vec3(3, 0, -12))
		.SetRotation(glm::angleAxis(glm::radians(180.f), BLM::GLMup));
	
	Rendering::LightCount = 1;
	Rendering::LightsPos[0] = glm::vec3(1.75f, -0.5f, 7.f);
	Rendering::frameEffects = &m_frameEffects;

	m_frameEffects.Init(windowWidth, windowHeight);
}

//lol is to avoid overwriting existing function lol
template<class T>
T fakeSmoothStep(T a, T b, float percent) {
	percent = glm::smoothstep(0.f, 1.f, percent);
	return (1 - percent) * a + percent * b;
}

void LeaderBoard::Update()
{
	float /*lx = 0, ly = 0,*/ rx = 0, ry = 0;
	for (int i(0); i < 4; ++i) {
		if (ControllerInput::GetButtonDown(BUTTON::START, CONUSER(i))) {
			if (ControllerInput::GetButton(BUTTON::RB, CONUSER(i))) {
				if (BackEnd::GetFullscreen())	BackEnd::SetTabbed();
				else							BackEnd::SetFullscreen();
			}
		}
	}

	auto& pTrans = ECS::GetComponent<Transform>(playerEnt);

	if (m_timer > 0) {
		m_timer -= Time::dt;
		if (m_timer <= 0) {
			m_timer = 0;
			//anything else here
		}
		if (m_timer < 1) {
			//press A to continue here
			ECS::GetComponent<Sprite>(text).SetWidth(fakeSmoothStep(-4.f, 0.f, m_timer));
			pTrans.SetPosition(fakeSmoothStep(glm::vec3(1.75f, -0.1f, -3.5f), glm::vec3(3, 0, -12), m_timer));
		}
		for (int i(0); i < 4; ++i) {
			//move the things
			if (playerScores[i].finalPos != BLM::GLMzero) {
				if (m_timer < 1) {
					ECS::GetComponent<Sprite>(playerScores[i].bar).SetWidth(fakeSmoothStep(-3.88f, 0.f, m_timer));
				}
				if (m_timer < 2) {
					//lerp
					ECS::GetComponent<Transform>(playerScores[i].parent).SetPosition(fakeSmoothStep(
						playerScores[i].finalPos, playerScores[i].startingPos, glm::clamp(m_timer - 1, 0.f, 1.f)
					));
				}
				if (m_timer < 3) {
					//display digits (takes a second)
					float percent = (m_timer - 2) * 2.f;
					ECS::GetComponent<Sprite>(playerScores[i].digit2).SetWidth(-0.75f * glm::clamp(2.f - percent, 0.f, 1.f));
					ECS::GetComponent<Sprite>(playerScores[i].digit1).SetWidth(-0.75f * glm::clamp(1.f - percent, 0.f, 1.f));
				}
			}
		}
		return;
	}

	for (int i(0); i < 4; ++i) {
		rx += ControllerInput::GetRX(CONUSER(i));
		ry += ControllerInput::GetRY(CONUSER(i));

		if (i < LeaderBoard::playerCount) {
			float lx = ControllerInput::GetLX(players[playerIndexes[i]].user);
			float ly = ControllerInput::GetLY(players[playerIndexes[i]].user);

			ECS::GetComponent<Transform>(playerScores[i].parent).SetRotation(
				glm::angleAxis(glm::radians(lx * -2.f), glm::vec3(0, -1, 0))
				* glm::angleAxis(glm::radians(ly * -2.f), glm::vec3(1, 0, 0)));
		}


		if (ControllerInput::GetButtonDown(BUTTON::A, CONUSER(i))) {

			QueueSceneChange(0);
		}
	}

	ECS::GetComponent<Transform>(camera).SetRotation(glm::angleAxis(glm::radians(rx), glm::vec3(0, -1, 0))
		* glm::angleAxis(glm::radians(ry * 0.5f), glm::vec3(1, 0, 0)));

	pTrans.SetRotation(pTrans.GetLocalRotation() * glm::angleAxis(Time::dt, BLM::GLMup));
}

void LeaderBoard::Exit()
{
	//reset all entity positions
	ECS::GetComponent<Transform>(camera).SetRotation(BLM::GLMQuat);
	ECS::GetComponent<Transform>(playerEnt).SetPosition(glm::vec3(3, 0, -12))
		.SetRotation(glm::angleAxis(glm::radians(180.f), BLM::GLMup));
	ECS::GetComponent<Sprite>(text).SetWidth(0);
	for (int i(0); i < 4; ++i) {
		ECS::GetComponent<Sprite>(playerScores[i].bar).SetWidth(0);
		ECS::GetComponent<Sprite>(playerScores[i].digit1).SetWidth(0);
		ECS::GetComponent<Sprite>(playerScores[i].digit2).SetWidth(0);
		ECS::GetComponent<Transform>(playerScores[i].parent).SetPosition(glm::vec3(0, 100, 0)).SetRotation(BLM::GLMQuat);
		playerScores[i].startingPos = BLM::GLMzero;
		playerScores[i].finalPos = BLM::GLMzero;
		players[i].score = 0;
	}

	Scene::Exit();
}

Scene* LeaderBoard::Reattach()
{
	Scene::Reattach();

	Rendering::LightCount = 1;
	Rendering::LightsPos[0] = glm::vec3(1.75f, -0.5f, 7.f);
	Rendering::BackColour = glm::vec4(0.5f, 0.5f, 1.f, 1.f);

	m_timer = 3.25f;

	playerIndexes.clear();

	if (LeaderBoard::playerCount == 0)	return this;

	//add score indexes
	for (int i(0), tempCounter(0); i < 4; ++i) {
		if (players[i].user != CONUSER::NONE)
			playerIndexes.push_back(i);
	}
	//sorts playerIndexes using splitting
	SortPlayers(playerIndexes);
	
	//set final pos here (final pos is based on actual order)
	for (int i(0); i < LeaderBoard::playerCount; ++i) {
		ECS::GetComponent<Sprite>(playerScores[i].number).Init(
			"num/" + std::to_string(playerIndexes[i] + 1) + ".png", -0.75f, 1.f);
		SetDigits(players[playerIndexes[i]].score, i);

		ECS::GetComponent<Transform>(playerScores[i].parent).SetPosition(
			playerScores[i].startingPos = glm::vec3(0, 1.5f - playerIndexes[i] * 1.25f, -8));

		playerScores[i].finalPos = glm::vec3(-2.5f, 1.5f - i * 1.25f, i * -0.5f - 7.f);
	}

	ECS::GetComponent<Player>(playerEnt).Init(CONUSER::NONE,
		LeaderBoard::players[playerIndexes[0]].model, LeaderBoard::players[playerIndexes[0]].colour);

	return this;
}

void LeaderBoard::SetDigits(int number, int index)
{
	if (number > 99)	return;
	if (index < 0 || index > 3)	return;

	int val = number / 10;
	ECS::GetComponent<Sprite>(playerScores[index].digit1).Init(
		"num/" + std::to_string(number - val * 10) + ".png", 0.f, 1.f);
	ECS::GetComponent<Sprite>(playerScores[index].digit2).Init("num/" + std::to_string(val) + ".png", 0.f, 1.f);
}

void LeaderBoard::SortPlayers(std::vector<int>& playerIndices)
{
	int size = playerIndices.size() - 1;
	//1 player would be 0, 2 player minimum
	if (size) {
		//simple sorting:
		for (int i(0); i < size; ++i) {
			for (int j(0); j < size; ++j) {
				if (players[playerIndices[j]].score < players[playerIndices[j + 1]].score) {
					int temp = playerIndices[j];
					playerIndices[j] = playerIndices[j + 1];
					playerIndices[j + 1] = temp;
				}
			}
		}
	}
}
