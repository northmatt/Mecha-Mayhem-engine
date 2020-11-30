#pragma once
#include "Utilities/Scene.h"

class DemoScene : public Scene
{
public:
	DemoScene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void Exit() override;
	virtual Scene* Reattach() override {
		if (m_camCount == 2)
			Player::SetUIAspect(width / 2.f, height);
		else
			Player::SetUIAspect(width, height);
		Player::SetCamDistance(camDistance);
		return Scene::Reattach();
	}

	void ShootLazer(entt::entity hitTarget, float width, glm::quat rotation, glm::vec3 pos, bool isp1);
	void updateLazer();

private:

	int width = 0;
	int height = 0;

	float camDistance = 5.f;
	glm::quat startQuat = glm::quat(-1, 0, 0, 0);

	bool screen = true;
	bool swap = false;

	entt::entity Lazer = entt::null;
	entt::entity Lazer2 = entt::null;

	entt::entity cameraEnt = 0;
	entt::entity cameraEnt2 = 0;
	entt::entity bodyEnt = 0;
	entt::entity bodyEnt2 = 0;
	entt::entity P = 0;
	entt::entity P2 = 0;

	entt::entity drone = 0;

	entt::entity epic = 0;

	Sound2D ouch{ "oof.mp4.mp3", "sfx" };
};

