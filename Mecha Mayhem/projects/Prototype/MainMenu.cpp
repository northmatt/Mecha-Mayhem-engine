#include "MainMenu.h"

void MainMenu::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);
	auto camera = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(camera).SetFovDegrees(60.f);
	ECS::GetComponent<Transform>(camera).SetPosition(glm::vec3(0.f, 0.f, 0.f));

	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjMorphLoader>(entity).LoadMeshs("title", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0.f, 2.5f, -10.f)).SetScale(2.f);
	}
}

void MainMenu::Update()
{
}

void MainMenu::Exit()
{
}
