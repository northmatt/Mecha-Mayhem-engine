#pragma once
#include "Utilities/Scene.h"
#include "Utilities/Catmull.h"

class Tutorial : public Scene
{
public:
	Tutorial(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f));

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void DrawOverlay() override;
	virtual void Exit() override {
		//SoundEventManager::ThreeDSounds = false;
		Scene::Exit();
	}
	virtual Scene* Reattach() override;

private:
	void SetDigits(int index, int number);

	CatmullFollower dronePath = { 3, {
		glm::vec3(0, 7, 0),
		glm::vec3(1, 7, -5),
		glm::vec3(4, 7, -10),
		glm::vec3(2, 9, -30),
		glm::vec3(0, 9, -50),
		glm::vec3(-2, 9, -30),
		glm::vec3(-4, 7, -10),
		glm::vec3(-1, 7, -5) }, 8
	};
	CatmullFollower dronePath2 = { 3, {
		glm::vec3(2, 9, -30),
		glm::vec3(0, 9, -50),
		glm::vec3(-2, 9, -30),
		glm::vec3(-4, 7, -10),
		glm::vec3(-1, 7, -5),
		glm::vec3(0, 7, 0),
		glm::vec3(1, 7, -5),
		glm::vec3(4, 7, -10)},8
	};
	CatmullFollower dronePath3 = { 3, {
		glm::vec3(-2, 9, -30),
		glm::vec3(-4, 7, -10),
		glm::vec3(0, 7, 0),
		glm::vec3(1, 7, -5),
		glm::vec3(4, 7, -10),
		glm::vec3(-1, 7, -5),
		glm::vec3(2, 9, -30),
		glm::vec3(0, 9, -50) },8
	};

	float camDistance = 2.5f;
	float m_timer = 0.f;

	struct DummyData {
		entt::entity dummy = entt::null;
		entt::entity digit1 = entt::null;
		entt::entity digit2 = entt::null;

		short lastHealth = 100;
		short currHealth = 0;

		float timer = 0;
	};

	DummyData dummies[6] = {};

	entt::entity bodyEnt[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity Head[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity cameraEnt[4] = { entt::null, entt::null, entt::null, entt::null };

	entt::entity lightDrone = entt::null;
	entt::entity cameraDrone = entt::null;
	entt::entity speakerDrone = entt::null;

	Sprite m_pauseSprite;
};

