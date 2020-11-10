#pragma once
#include "Utilities/Scene.h"

class DemoScene : public Scene
{
public:
	DemoScene(const std::string& name, glm::vec3 gravity = glm::vec3(0.f), bool physics = true) :
		Scene(name, gravity, physics) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;

	void GetWindow(GLFWwindow* win) { window = win; }

private:
	//want to outsource all window stuff outside of the scenes
	GLFWwindow* window = nullptr;

	int width = 0;
	int height = 0;


	float pi = glm::half_pi<float>() - 0.01f;
	glm::quat startQuat = glm::quat(-1, 0, 0, 0);
	glm::vec2 rot = glm::vec2(0.f);

	bool buttonPressed = false;
	bool screen = true;

	unsigned cameraEnt = 0;
	unsigned bodyEnt = 0;
	unsigned Dio = 0;
	unsigned Dio2 = 0;
	unsigned Dio3 = 0;

	std::vector<unsigned> someObjs = {};

};

