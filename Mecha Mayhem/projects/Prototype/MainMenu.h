#pragma once
#include "Utilities/Scene.h"
class MainMenu : public Scene
{
public:
	MainMenu(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}
	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void Exit() override;
private:
	bool m_exit = false;
};

