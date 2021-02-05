#pragma once
#include "Utilities/Scene.h"

class MapEditor : public Scene
{
public:
	MapEditor(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void Exit() override;
	/*virtual Scene* Reattach() override {

		return Scene::Reattach();
	}*/

private:
	int width = 0;
	int height = 0;

	bool saveOnExit = false;

	entt::entity bodyEnt1 = entt::null;
	entt::entity Head1 = entt::null;
	entt::entity cameraEnt1 = entt::null;
};

