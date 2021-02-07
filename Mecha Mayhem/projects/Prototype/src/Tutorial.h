#pragma once
#include "Utilities/Scene.h"
#include "Utilities/Catmull.h"
#include "GAss/ColorCorrection.h"

class Tutorial : public Scene
{
public:
	Tutorial(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual Scene* Reattach() override;

	
	void SetCubeFiles(LUT3D warmCube, LUT3D coldCube, LUT3D customCube);

private:
	
	//CG ASS.
	//TODO: Get Cube file
	LUT3D m_warmCube;
	LUT3D m_coldCube;
	LUT3D m_CustomCube;
		  
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
	CatmullFollower dronePath4 = { 3, {
		glm::vec3(-1, 5, 1),
		glm::vec3(1, 5, 1),
		glm::vec3(1, 5, -1),
		glm::vec3(-1, 5, -1) },8
	};


	int width = 0;
	int height = 0;

	size_t killGoal = 11;

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

	/*entt::entity lightDrone = entt::null;
	entt::entity cameraDrone = entt::null;
	entt::entity speakerDrone = entt::null;
	entt::entity shieldDrone = entt::null;*/
};

