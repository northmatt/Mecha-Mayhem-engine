#include "DemoScene.h"
#include "Utilities/BLM.h"

void DemoScene::Init(int windowWidth, int windowHeight)
{
	//generic init, use as guidelines
	ECS::AttachRegistry(&m_reg);
	if (m_world)
		PhysBody::Init(m_world);

	width = windowWidth;
	height = windowHeight;


	/// Creating Entities
	cameraEnt = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
	camCam.SetFovDegrees(60.f).ResizeWindow(width, height);

	bodyEnt = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt).Init(0.5f, 2, glm::vec3(0, 0, 0), 1, true).GetBody()->setAngularFactor(btVector3(0, 0, 0));
	ECS::GetComponent<Transform>(cameraEnt).ChildTo(bodyEnt).SetPosition(glm::vec3(0, 1, 0));

	ECS::AttachComponent<PhysBody>(ECS::CreateEntity()).Init(100, 1, 100, glm::vec3(0, -30, 0));

	Dio = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio).LoadMesh("models/Char.obj", true);
	ECS::AttachComponent<PhysBody>(Dio).Init(10, 1, 10, glm::vec3(0, 30, 0));
	ECS::GetComponent<Transform>(Dio).SetPosition(glm::vec3(0, 30, 0));

	Dio2 = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio2).LoadMesh("models/Char.obj", true);
	ECS::GetComponent<Transform>(Dio2).SetPosition(glm::vec3(0, 10, 0)).ChildTo(Dio);

	Dio3 = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio3).LoadMesh("models/Pistol.obj", true);
	ECS::GetComponent<Transform>(Dio3).SetPosition(glm::vec3(1.5f, -2, -2)).ChildTo(cameraEnt).SetScale(glm::vec3(0.25f));
	{
		unsigned P = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(P).LoadMesh("models/Pistol.obj");
		ECS::GetComponent<Transform>(P).SetPosition(glm::vec3(0, 0, 0)).ChildTo(bodyEnt).SetScale(glm::vec3(0.1f));
	}
	{
		unsigned entity = ECS::CreateEntity();
		auto& temp = ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/cringe.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -20, 0));
	}

	unsigned amt = 100;

	for (int count(0); count < amt; ++count) {
		someObjs.push_back(ECS::CreateEntity());
		ECS::AttachComponent<ObjLoader>(someObjs[count]).LoadMesh("models/blade.obj", count % 2);

		auto& trans = ECS::GetComponent<Transform>(someObjs[count]);

		glm::vec3 axis = glm::vec3(rand() % 2, rand() % 2, rand() % 2 + 1);
		trans.SetPosition(glm::vec3(rand() % 21 - 10, rand() % 21 - 10, rand() % 21 - 10)).
			SetScale(glm::vec3((rand() % 8 + 3) / 10.f)).
			SetRotation(glm::rotate(glm::quat(1.f, 0, 0, 0), float(rand() % 8 - 3), axis));
	}

	/// End of creating entities
}

void DemoScene::Update()
{
	auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);
	auto& ballPhys = ECS::GetComponent<PhysBody>(bodyEnt);

	/// start of loop
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		if (!buttonPressed) {
			buttonPressed = true;
			if (screen = !screen)
				BackEnd::SetTabbed(width, height);
			else
				BackEnd::SetFullscreen();
		}
	}
	else buttonPressed = false;

	if (glfwGetKey(window, GLFW_KEY_UP)) {
		rot.x += 3.f * m_dt;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		rot.x -= 3.f * m_dt;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		rot.y -= 3.f * m_dt;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		rot.y += 3.f * m_dt;
	}
	if (rot.x > pi) {
		rot.x = pi;
	}
	else if (rot.x < -pi) {
		rot.x = -pi;
	}
	{
		//glm::quat rotf = glm::rotate(startQuat, rot.y, glm::vec3(0, -1, 0));
		//rotf = glm::rotate(rotf, rot.x, glm::vec3(1, 0, 0));
		ballPhys.SetRotation(glm::rotate(startQuat, rot.y, glm::vec3(0, -1, 0)));
		camTrans.SetRotation(glm::rotate(startQuat, rot.x, glm::vec3(1, 0, 0)));
	}
	/*glm::mat4 rotf = glm::rotate(glm::mat4(1.f), rot.x, glm::vec3(1, 0, 0));
	rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
	camTrans.SetRotation(glm::mat3(rotf));*/

	glm::vec3 pos2 = glm::vec3(0.f);
	if (glfwGetKey(window, GLFW_KEY_I)) {
		pos2.z -= 15 * m_dt;
	}
	if (glfwGetKey(window, GLFW_KEY_K)) {
		pos2.z += 15 * m_dt;
	}
	if (glfwGetKey(window, GLFW_KEY_J)) {
		pos2.x -= 15 * m_dt;
	}
	if (glfwGetKey(window, GLFW_KEY_L)) {
		pos2.x += 15 * m_dt;
	}
	ECS::GetComponent<PhysBody>(Dio).SetPosition(
		ECS::GetComponent<Transform>(Dio).GetPosition() + pos2);
	ECS::GetComponent<Transform>(Dio2).SetPosition(
		ECS::GetComponent<Transform>(Dio2).GetPosition() + pos2);

	glm::vec3 pos = glm::vec3(0.f);
	if (glfwGetKey(window, GLFW_KEY_W)) {
		pos.z -= 15;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		pos.z += 15;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		pos.x -= 15;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		pos.x += 15;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		pos.y -= 15;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		pos.y += 50;
	}
	if (pos.x != 0 || pos.y != 0 || pos.z != 0) {
		pos = glm::vec4(pos, 1) * glm::rotate(glm::mat4(1.f), rot.y, glm::vec3(0, 1, 0));
		//camTrans.SetPosition(camTrans.GetPosition() + pos);
		btVector3 temp = ballPhys.GetVelocity();
		if (pos.x != 0)		temp.setX(pos.x);
		if (pos.y != 0)		temp.setY(pos.y);
		if (pos.z != 0)		temp.setZ(pos.z);
		ballPhys.SetVelocity(temp);
		ballPhys.SetAwake();
	}


	if (glfwGetKey(window, GLFW_KEY_E)) {
		if (someObjs.size() > 0) {
			ECS::DestroyEntity(someObjs[0]);
			someObjs.erase(someObjs.begin());
		}
	}
	if (glfwGetKey(window, GLFW_KEY_P)) {
		for (int count(0); count < someObjs.size(); ++count) {
			ECS::GetComponent<Transform>(someObjs[count]).SetPosition(
				glm::vec3(rand() % 21 - 10, rand() % 21 - 10, rand() % 21 - 10)).
				SetScale(glm::vec3((rand() % 8 + 3) / 10.f));
		}
		Rendering::DefaultColour = glm::vec3((rand() % 10) / 10.f, (rand() % 10) / 10.f, (rand() % 10) / 10.f);
		Rendering::LightColour = glm::vec3((rand() % 30) / 10.f, (rand() % 30) / 10.f, (rand() % 30) / 10.f);
	}

	/// End of loop
}
