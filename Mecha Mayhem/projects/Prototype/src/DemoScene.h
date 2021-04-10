#pragma once
#include "Utilities/Scene.h"

class DemoScene : public Scene
{
public:
	DemoScene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f));

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void DrawOverlay() override;
	virtual Scene* Reattach() override;
	virtual void ImGuiFunc() override;
	virtual void Exit() override {
		m_gameTimer = -1.f;
		//impossible goal (if it does happen, then the game jsut ends)
		killGoal = 100;
	}

	static LUT3D gameEndCube;
private:
	static const float winWaitTime;
	float m_yPos = 8.5f;
	size_t killGoal = 100;

	float camDistance = 2.5f;
	float m_timer = 0.f;
	float m_gameTimer = -1.f;

	entt::entity bodyEnt[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity Head[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity cameraEnt[4] = { entt::null, entt::null, entt::null, entt::null };

	Sprite m_pauseSprite;
	Sprite m_colonSprite;
};

inline const float DemoScene::winWaitTime = 5.f;
inline LUT3D DemoScene::gameEndCube;

