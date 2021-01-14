#include "Algo.h"

void Algo::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);

	m_camera = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(m_camera).SetFovDegrees(60.f)
		.ResizeWindow(windowWidth, windowHeight).SetNear(0.3f);

	m_model = ECS::CreateEntity();
	ECS::AttachComponent<ObjMorphLoader>(m_model).LoadMeshs("toilet", true).LoadMeshs("quagmire", true);
	ECS::GetComponent<Transform>(m_model).SetPosition(glm::vec3(0.f, 0.f, -5.f));

	Rendering::LightCount = 1;
	Rendering::LightsColour[0] = glm::vec3(3.f);
}

void Algo::Update()
{
	auto &camTrans = ECS::GetComponent<Transform>(m_camera);

	if (Input::GetKeyDown(KEY::ONE)) {
		if (ECS::GetComponent<ObjMorphLoader>(m_model).ToggleBounce().GetBounce())
			std::cout << "bouncy     \r";
		else
			std::cout << "not bouncy \r";
	}
	if (Input::GetKeyDown(KEY::TWO)) {
		if (ECS::GetComponent<ObjMorphLoader>(m_model).ToggleDirection().GetDirection())
			std::cout << "forwards   \r";
		else
			std::cout << "backwards  \r";
	}
	if (Input::GetKeyDown(KEY::THREE)) {
		if (ECS::GetComponent<ObjMorphLoader>(m_model).ToggleLooping().GetLooping())
			std::cout << "looping    \r";
		else
			std::cout << "not looping\r";
	}
	if (Input::GetKeyDown(KEY::FOUR)) {
		m_anim1 = !m_anim1;
	}
	if (m_anim1)
		ECS::GetComponent<ObjMorphLoader>(m_model).BlendTo("quagmire", 5);
	else
		ECS::GetComponent<ObjMorphLoader>(m_model).BlendTo("toilet", 1);

	int rotx = Input::GetKey(KEY::UP) - Input::GetKey(KEY::DOWN);
	int roty = Input::GetKey(KEY::RIGHT) - Input::GetKey(KEY::LEFT);

	if (rotx != 0 || roty != 0) {
		m_rotx += rotx * Time::dt;
		m_roty += roty * Time::dt;

		glm::quat rotation = glm::rotate(BLM::GLMQuat, -m_roty, BLM::GLMup);
		rotation = glm::rotate(rotation, m_rotx, glm::vec3(1, 0, 0));
		camTrans.SetRotation(rotation);
	}

	int x = Input::GetKey(KEY::D) - Input::GetKey(KEY::A);
	int z = Input::GetKey(KEY::S) - Input::GetKey(KEY::W);
	int y = Input::GetKey(KEY::SPACE) - Input::GetKey(KEY::LSHIFT);

	if (x != 0 || y != 0 || z != 0) {
		glm::vec3 pos = glm::vec4(x, y, z, 1) * glm::rotate(glm::mat4(1.f), m_roty, BLM::GLMup);
		camTrans.SetPosition(camTrans.GetLocalPosition() + pos * 3.f * Time::dt);
	}

	Rendering::LightsPos[0] = camTrans.GetLocalPosition();
}
