#pragma once
#include "Utilities/Scene.h"
#include "Effects/Post/BloomEffect.h"

class MapEditor : public Scene
{
public:
	MapEditor(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void Exit() override;
	virtual Scene* Reattach() override;
	virtual void ImGuiFunc() override;

private:
	int width = 0;
	int height = 0;

	float speed = 5;
	float rotSpeed = 2.5f;
	std::string debugText = "";
	glm::vec3 rot = glm::vec3(0.f);

	bool saveOnExit = false;
	bool showRay = false;

	entt::entity rayPosEnt = entt::null;
	entt::entity cameraEnt = entt::null;
};

