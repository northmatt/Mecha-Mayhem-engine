#include "DemoScene.h"

void DemoScene::Init(int windowWidth, int windowHeight)
{
	//generic init, use as guidelines
	ECS::AttachRegistry(&m_reg);
	if (m_world)
		PhysBody::Init(m_world);

	width = windowWidth;
	height = windowHeight;


	/// Creating Entities
	for (int x(0); x < 3; ++x)
	{
		unsigned cameraEnt2 = ECS::CreateEntity();
		auto& camCam2 = ECS::AttachComponent<Camera>(cameraEnt2);
		camCam2.SetFovDegrees(60.f).ResizeWindow(width, height);
		glm::vec3 axis = glm::vec3(rand() % 2, rand() % 2, rand() % 2 + 1);
		ECS::GetComponent<Transform>(cameraEnt2).
			SetPosition(glm::vec3(rand() % 21 - 10, 0, rand() % 21 - 10)).
			SetRotation(glm::rotate(glm::quat(1, 0, 0, 0), float(rand() % 360), axis));
	}
	cameraEnt = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
	camCam.SetFovDegrees(60.f).ResizeWindow(width, height);

	bodyEnt = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt).Init(0.5f, 2, glm::vec3(0, 0, 0), 1, true).GetBody()->setAngularFactor(btVector3(0, 0, 0));
	ECS::AttachComponent<ObjLoader>(bodyEnt).LoadMesh("models/Pistol.obj");
	ECS::GetComponent<Transform>(bodyEnt).SetScale(0.1f);

	{
		unsigned god = ECS::CreateEntity();
		ECS::AttachComponent<PhysBody>(god).Init(1000, 1, 1000, glm::vec3(0, -24, 0));
		ECS::AttachComponent<ObjLoader>(god).LoadMesh("models/GodHimself.obj");
		ECS::GetComponent<Transform>(god).SetScale(glm::vec3(1000, 1, 1000));
	}

	/*unsigned charr = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(char).LoadMesh("models/Char.obj", true);
	ECS::AttachComponent<PhysBody>(char).Init(10, 1, 10, glm::vec3(0, 30, 0));
	ECS::GetComponent<Transform>(char).SetPosition(glm::vec3(0, 30, 0)).SetScale(3.f);

	unsigned charr2 = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(charr2).LoadMesh("models/Char.obj", true);
	ECS::GetComponent<Transform>(charr2).SetPosition(glm::vec3(0, 10, 0)).ChildTo(charr);*/

	Dio = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio).LoadMesh("models/Pistol.obj", true);

	P = ECS::CreateEntity();
	ECS::GetComponent<Transform>(P).SetPosition(glm::vec3(0, 1, 0)).ChildTo(bodyEnt).SetScale(10.f);
	ECS::GetComponent<Transform>(Dio).SetPosition(glm::vec3(0.5f, 0, -0.5f)).ChildTo(P).SetScale(0.25f);
	ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(-0.5f, 0, 5)).ChildTo(P);
	{
		unsigned entity = ECS::CreateEntity();
		auto& temp = ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/cringe.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -30, 0)).SetScale(4.f);
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

	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 1.f);
	/// End of creating entities
}

void DemoScene::Update()
{
	//auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);
	auto& ballPhys = ECS::GetComponent<PhysBody>(bodyEnt);

	/// start of loop
	
	//if (Input::GetKeyDown(KEY::ESC)) {
	if (Input::GetKeyDown(KEY::ESC)) {
		if (Input::GetKey(KEY::LSHIFT)) {
			if (screen = !screen)
				BackEnd::SetTabbed(width, height);
			else
				BackEnd::SetFullscreen();
		}
		else {
			m_camCount++;
			if (m_camCount > 4)
				m_camCount = 1;
			if (m_camCount == 2)
				ECS::GetComponent<Camera>(cameraEnt).ResizeWindow(width / 2, height);
			else
				ECS::GetComponent<Camera>(cameraEnt).ResizeWindow(width, height);
		}
	}

	if (Input::GetKey(KEY::UP)) {
		rot.x += 3.f * m_dt;
	}
	if (Input::GetKey(KEY::DOWN)) {
		rot.x -= 3.f * m_dt;
	}
	if (Input::GetKey(KEY::LEFT)) {
		rot.y -= 3.f * m_dt;
	}
	if (Input::GetKey(KEY::RIGHT)) {
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
		ECS::GetComponent<Transform>(P).SetRotation(glm::rotate(startQuat, rot.x, glm::vec3(1, 0, 0)));
	}
	/*glm::mat4 rotf = glm::rotate(glm::mat4(1.f), rot.x, glm::vec3(1, 0, 0));
	rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
	camTrans.SetRotation(glm::mat3(rotf));*/

	/*glm::vec3 pos2 = glm::vec3(0.f);
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
	}*/

	glm::vec3 pos = glm::vec3(0.f);
	if (Input::GetKey(KEY::W)) {
		pos.z -= 15;
	}
	if (Input::GetKey(KEY::S)) {
		pos.z += 15;
	}
	if (Input::GetKey(KEY::A)) {
		pos.x -= 15;
	}
	if (Input::GetKey(KEY::D)) {
		pos.x += 15;
	}
	if (Input::GetKey(KEY::LSHIFT)) {
		pos.y -= 1;
		ballPhys.SetVelocity(btVector3(0.f, 0.f, 0.f));
	}
	if (Input::GetKey(KEY::SPACE)) {
		pos.y += 50 /*x at least two times this*/;
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


	if (Input::GetKey(KEY::E)) {
		if (someObjs.size() > 0) {
			ECS::DestroyEntity(someObjs[0]);
			someObjs.erase(someObjs.begin());
		}
	}
	if (Input::GetKey(KEY::P)) {
		for (int count(0); count < someObjs.size(); ++count) {
			ECS::GetComponent<Transform>(someObjs[count]).SetPosition(
				glm::vec3(rand() % 21 - 10, rand() % 21 - 10, rand() % 21 - 10)).
				SetScale(glm::vec3((rand() % 8 + 3) / 10.f));
		}
		if (Input::GetKey(KEY::LSHIFT))
			Rendering::LightColour = glm::vec3(3.f);
		else
			Rendering::LightColour = glm::vec3((rand() % 30) / 10.f, (rand() % 30) / 10.f, (rand() % 30) / 10.f);
	}

	/// End of loop
}
