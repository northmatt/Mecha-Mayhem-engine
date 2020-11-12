#pragma once
#include "Utilities/Scene.h"

class DemoScene : public Scene
{
public:
	DemoScene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;

private:

	int width = 0;
	int height = 0;


	float pi = glm::half_pi<float>() - 0.01f;
	glm::quat startQuat = glm::quat(-1, 0, 0, 0);
	glm::vec2 rot = glm::vec2(0.f);

	bool screen = true;

	unsigned cameraEnt = 0;
	unsigned bodyEnt = 0;
	unsigned Dio = 0;
	unsigned P = 0;
};

