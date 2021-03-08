#pragma once
#include "Utilities/Scene.h"

class DemoScene : public Scene
{
public:
	DemoScene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual Scene* Reattach() override;
	/*virtual void ImGuiFunc() override {
		for (int i(0); i < 4; ++i) {
			if (bodyEnt[i] != entt::null)
				if (ImGui::Button(("Give player " + std::to_string(i + 1) + " 1 kill").c_str()))
					ECS::GetComponent<Player>(bodyEnt[i]).GivePoints(1);
		}
	}*/

private:

	size_t killGoal = 10;

	float camDistance = 2.5f;
	float m_timer = 0.f;

	entt::entity bodyEnt[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity Head[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity cameraEnt[4] = { entt::null, entt::null, entt::null, entt::null };
};

