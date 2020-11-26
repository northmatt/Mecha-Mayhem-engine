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

	void ShootLazer(float width, glm::quat rotation, glm::vec3 pos, bool isp1);
	void updateLazer();

private:

	int width = 0;
	int height = 0;

	float camDistance = 5.f;
	float pi = glm::half_pi<float>() - 0.01f;
	glm::quat startQuat = glm::quat(-1, 0, 0, 0);
	glm::vec2 rot = glm::vec2(0.f);
	glm::vec2 rot2 = glm::vec2(0.f);

	bool screen = true;
	bool swap = false;

	int Lazer = -1;
	int Lazer2 = -1;

	unsigned cameraEnt = 0;
	unsigned cameraEnt2 = 0;
	unsigned bodyEnt = 0;
	unsigned bodyEnt2 = 0;
	unsigned bodyEntoff = 0;
	unsigned bodyEntoff2 = 0;
	unsigned P = 0;
	unsigned P2 = 0;

	unsigned drone = 0;

	unsigned epic = 0;

	Sound2D ouch{ "oof.mp4.mp3", "sfx" };
};

