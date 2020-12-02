#pragma once
#include "Utilities/Scene.h"

class Tutorial : public Scene
{
public:
	Tutorial(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual Scene* Reattach() override {
		if (m_camCount == 2)
			Player::SetUIAspect(width / 2.f, height);
		else
			Player::SetUIAspect(width, height);
		Player::SetCamDistance(camDistance);
		return Scene::Reattach();
	}

private:

	int width = 0;
	int height = 0;

	float camDistance = 2.5f;
	glm::quat startQuat = glm::quat(0, 0, 1, 0);

	entt::entity bodyEnt1 = entt::null;
	entt::entity Head1 = entt::null;
	entt::entity cameraEnt1 = entt::null;

	entt::entity bodyEnt2 = entt::null;
	entt::entity Head2 = entt::null;
	entt::entity cameraEnt2 = entt::null;

	entt::entity bodyEnt3 = entt::null;
	entt::entity Head3 = entt::null;
	entt::entity cameraEnt3 = entt::null;

	entt::entity bodyEnt4 = entt::null;
	entt::entity Head4 = entt::null;
	entt::entity cameraEnt4 = entt::null;
};

