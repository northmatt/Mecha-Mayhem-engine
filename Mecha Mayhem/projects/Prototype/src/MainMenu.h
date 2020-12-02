#pragma once
#include "Utilities/Scene.h"
#include "Utilities/Catmull.h"

class MainMenu : public Scene
{
public:
	MainMenu(const std::string& name) :
		Scene(name, BLM::GLMzero, false) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void Exit() override;

private:
	bool m_exit = false;

	float zoomTime = 1;

	CatmullFollower cameraPath{ 1, {
		glm::vec3(-2.5f, 2.5f, 12.5f),
		glm::vec3(12.5f, 0.f, 12.5f),
		glm::vec3(12.5f, 2.5f, -2.5f),
		glm::vec3(-2.5f, 0.f, -2.5f)
	}, 8 };

	entt::entity camera = entt::null;
	entt::entity title = entt::null;
	entt::entity text = entt::null;
};

