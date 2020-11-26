#include "DemoScene.h"

void DemoScene::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "testmap";
	std::cout << "filename: " + input + "\n";
	//std::cin >> input;
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";

	width = windowWidth;
	height = windowHeight;


	/// Creating Entities
	for (int x(0); x < 2; ++x)
	{
		auto entity = ECS::CreateEntity();
		auto& camCam2 = ECS::AttachComponent<Camera>(entity);
		camCam2.SetFovDegrees(60.f).ResizeWindow(width, height);
		glm::vec3 axis = glm::vec3(rand() % 2, rand() % 2, rand() % 2 + 1);
		ECS::GetComponent<Transform>(entity).
			SetPosition(glm::vec3(rand() % 21 - 10, 0, rand() % 21 - 10)).
			SetRotation(glm::rotate(glm::quat(1, 0, 0, 0), float(rand() % 360), axis));
	}
	cameraEnt2 = ECS::CreateEntity();
	auto& camCam2 = ECS::AttachComponent<Camera>(cameraEnt2);
	camCam2.SetFovDegrees(60.f).ResizeWindow(width, height);

	cameraEnt = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
	camCam.SetFovDegrees(60.f).ResizeWindow(width, height);

	bodyEnt = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt).Init(0.5f, 2, glm::vec3(0, 10, 0), 1, true).
		/*SetGravity(glm::vec3(0)).*/GetBody()->setAngularFactor(btVector3(0, 0, 0));
	ECS::GetComponent<PhysBody>(bodyEnt).GetBody()->setFriction(0);
	{
		bodyEntoff = ECS::CreateEntity();
		ECS::AttachComponent<ObjMorphLoader>(bodyEntoff).LoadMeshs("char/walk", true).LoadMeshs("char/idle", true);
		ECS::GetComponent<Transform>(bodyEntoff).ChildTo(bodyEnt).SetPosition(glm::vec3(0, -1, 0));
	}
	bodyEnt2 = ECS::CreateEntity();
	ECS::AttachComponent<PhysBody>(bodyEnt2).Init(0.5f, 2, glm::vec3(0, 10, 0), 1, true).
		/*SetGravity(glm::vec3(0)).*/GetBody()->setAngularFactor(btVector3(0, 0, 0));
	ECS::GetComponent<PhysBody>(bodyEnt2).GetBody()->setFriction(0);
	{
		bodyEntoff2 = ECS::CreateEntity();
		ECS::AttachComponent<ObjMorphLoader>(bodyEntoff2).LoadMeshs("char/walk", true).LoadMeshs("char/idle", true);
		ECS::GetComponent<Transform>(bodyEntoff2).ChildTo(bodyEnt2).SetPosition(glm::vec3(0, -1, 0));
	}

	drone = ECS::CreateEntity();
	ECS::AttachComponent<ObjMorphLoader>(drone).LoadMeshs("sword").LoadMeshs("shield").LoadMeshs("drone").LoadMeshs("othershield/anim", true).LoadMeshs("anotherOne");
	ECS::GetComponent<Transform>(drone).ChildTo(bodyEnt).SetPosition(glm::vec3(0, 0, 1.f)).SetScale(0.3f);

	auto Dio = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio).LoadMesh("models/Pistol.obj", true);

	auto Dio2 = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio2).LoadMesh("models/Pistol.obj", true);

	P = ECS::CreateEntity();
	ECS::GetComponent<Transform>(P).SetPosition(glm::vec3(0, 0.75f, 0)).ChildTo(bodyEnt);
	ECS::GetComponent<Transform>(Dio).SetPosition(glm::vec3(0.4f, 0.25f, -0.5f)).
		ChildTo(bodyEnt).SetUsingParentScale(false).SetScale(0.05f);
	ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(P).SetUsingParentScale(false);

	P2 = ECS::CreateEntity();
	ECS::GetComponent<Transform>(P2).SetPosition(glm::vec3(0, 0.75f, 0)).ChildTo(bodyEnt2);
	ECS::GetComponent<Transform>(Dio2).SetPosition(glm::vec3(0.4f, 0.25f, -0.5f)).
		ChildTo(bodyEnt2).SetUsingParentScale(false).SetScale(0.05f);
	ECS::GetComponent<Transform>(cameraEnt2).SetPosition(glm::vec3(0, 0, camDistance)).
		ChildTo(P2).SetUsingParentScale(false);


	{
		unsigned entity = ECS::CreateEntity();
		ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/cringe.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -30, 0)).SetScale(4.f);
	}

	/// End of creating entities
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 1.f);
	Rendering::hitboxes = &m_colliders;

	//cube
	epic = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(epic).LoadMesh("models/GodHimself.obj");

}

void DemoScene::Update()
{
	//auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);
	auto& ballPhys = ECS::GetComponent<PhysBody>(bodyEnt).SetAwake();
	auto& ballPhys2 = ECS::GetComponent<PhysBody>(bodyEnt2).SetAwake();
	/// start of loop

	if (Input::GetKey(KEY::APOSTROPHE)) {
		ECS::GetComponent<Transform>(drone).SetRotation(glm::rotate(ECS::GetComponent<Transform>(drone).
			GetLocalRotation(), glm::radians(45 * m_dt), glm::vec3(0, 1, 0))).SetPosition(
				glm::vec3(0, 0, 1.f) * glm::inverse(ECS::GetComponent<Transform>(drone).GetLocalRotation())
			);
	}
	if (Input::GetKey(KEY::SEMICOLON)) {
		ECS::GetComponent<Transform>(drone).SetRotation(glm::rotate(ECS::GetComponent<Transform>(drone).
			GetLocalRotation(), glm::radians(45 * m_dt), glm::vec3(0, -1, 0))).SetPosition(
				glm::vec3(0, 0, 1.f) * glm::inverse(ECS::GetComponent<Transform>(drone).GetLocalRotation())
			);
	}

	if (Input::GetKeyDown(KEY::ONE)) {
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("sword");
	}
	if (Input::GetKeyDown(KEY::TWO)) {
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("shield");
	}
	if (Input::GetKeyDown(KEY::THREE)) {
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("drone");
	}
	if (Input::GetKeyDown(KEY::FOUR)) {
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("othershield/anim", true);
	}
	if (Input::GetKeyDown(KEY::FIVE)) {
		ECS::GetComponent<ObjMorphLoader>(drone).ToggleDirection();
	}
	if (Input::GetKeyDown(KEY::SIX)) {
		//ECS::GetComponent<ObjMorphLoader>(drone).ToggleBounce();
		ECS::GetComponent<ObjMorphLoader>(drone).LoadMeshs("anotherOne");
	}

	if (Input::GetKeyDown(KEY::ESC)) {
		if (Input::GetKey(KEY::LSHIFT)) {
			if (screen = !screen)	BackEnd::SetTabbed(width, height);
			else					BackEnd::SetFullscreen();
		}
		else {
			if (++m_camCount > 4)	m_camCount = 1;
			if (m_camCount == 2) {
				ECS::GetComponent<Camera>(cameraEnt).ResizeWindow(width / 2, height);
				ECS::GetComponent<Camera>(cameraEnt2).ResizeWindow(width / 2, height);
			}
			else {
				ECS::GetComponent<Camera>(cameraEnt).ResizeWindow(width, height);
				ECS::GetComponent<Camera>(cameraEnt2).ResizeWindow(width, height);
			}
		}
	}

	{	//p1 movement
		if (Input::GetKey(KEY::UP))			rot.x += 3.f * m_dt;
		if (Input::GetKey(KEY::DOWN))		rot.x -= 3.f * m_dt;
		if (Input::GetKey(KEY::LEFT))		rot.y -= 3.f * m_dt;
		if (Input::GetKey(KEY::RIGHT))		rot.y += 3.f * m_dt;

		rot.x += ControllerInput::GetRY(CONUSER::ONE) * 3.f * m_dt;
		rot.y += ControllerInput::GetRX(CONUSER::ONE) * 3.f * m_dt;

		if (rot.x > pi)			rot.x = pi;
		else if (rot.x < -pi)	rot.x = -pi;

		{
			//glm::quat rotf = glm::rotate(startQuat, rot.y, glm::vec3(0, -1, 0));
			//rotf = glm::rotate(rotf, rot.x, glm::vec3(1, 0, 0));
			ballPhys.SetRotation(glm::rotate(startQuat, rot.y, glm::vec3(0, -1, 0)));
			ECS::GetComponent<Transform>(P).SetRotation(glm::rotate(startQuat, rot.x, glm::vec3(1, 0, 0))).ComputeGlobal();
		}
		/*glm::mat4 rotf = glm::rotate(glm::mat4(1.f), rot.x, glm::vec3(1, 0, 0));
		rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
		camTrans.SetRotation(glm::mat3(rotf));*/

		glm::vec3 pos = glm::vec3(0.f);
		pos.y = ballPhys.GetVelocity().y();
		if (Input::GetKey(KEY::W))		pos.z -= 15;
		if (Input::GetKey(KEY::S))		pos.z += 15;
		if (Input::GetKey(KEY::A))		pos.x -= 15;
		if (Input::GetKey(KEY::D))		pos.x += 15;

		pos.x += ControllerInput::GetLXRaw(CONUSER::ONE) * 15;
		pos.z -= ControllerInput::GetLYRaw(CONUSER::ONE) * 15;

		if ((Input::GetKey(KEY::LSHIFT) || ControllerInput::GetButton(BUTTON::B, CONUSER::ONE))&& pos.y < 500) {
			pos.y -= 15 * m_dt;
			//ballPhys.SetVelocity(btVector3(0.f, 0.f, 0.f));
		}
		if (Input::GetKeyDown(KEY::SPACE) || ControllerInput::GetButtonDown(BUTTON::A, CONUSER::ONE)) {
			pos.y = 35 /*x at least two times this*/;
			//pos.y += 15;
		}
		if (pos.x != 0 || pos.z != 0) {
			//if (!ECS::GetComponent<ObjMorphLoader>(bodyEnt).IsAnim("char/walk"))
			ECS::GetComponent<ObjMorphLoader>(bodyEntoff).BlendTo("char/walk");
		}
		else {
			//if (!ECS::GetComponent<ObjMorphLoader>(bodyEnt).IsAnim("char/idle"))
			ECS::GetComponent<ObjMorphLoader>(bodyEntoff).BlendTo("char/idle");
		}
		pos = glm::vec4(pos, 1) * glm::rotate(glm::mat4(1.f), rot.y, glm::vec3(0, 1, 0));
		ballPhys.SetVelocity(pos);
	}

	{	//p2 movement
		if (Input::GetKey(KEY::UP))			rot2.x += 3.f * m_dt;
		if (Input::GetKey(KEY::DOWN))		rot2.x -= 3.f * m_dt;
		if (Input::GetKey(KEY::LEFT))		rot2.y -= 3.f * m_dt;
		if (Input::GetKey(KEY::RIGHT))		rot2.y += 3.f * m_dt;

		rot2.x += ControllerInput::GetRY(CONUSER::TWO) * 3.f * m_dt;
		rot2.y += ControllerInput::GetRX(CONUSER::TWO) * 3.f * m_dt;

		if (rot2.x > pi)		rot2.x = pi;
		else if (rot2.x < -pi)	rot2.x = -pi;

		{
			//glm::quat rotf = glm::rotate(startQuat, rot.y, glm::vec3(0, -1, 0));
			//rotf = glm::rotate(rotf, rot.x, glm::vec3(1, 0, 0));
			ballPhys2.SetRotation(glm::rotate(startQuat, rot2.y, glm::vec3(0, -1, 0)));
			ECS::GetComponent<Transform>(P2).SetRotation(glm::rotate(startQuat, rot2.x, glm::vec3(1, 0, 0))).ComputeGlobal();
		}
		/*glm::mat4 rotf = glm::rotate(glm::mat4(1.f), rot.x, glm::vec3(1, 0, 0));
		rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
		camTrans.SetRotation(glm::mat3(rotf));*/

		glm::vec3 pos = glm::vec3(0.f);
		pos.y = ballPhys2.GetVelocity().y();
		if (Input::GetKey(KEY::W))		pos.z -= 15;
		if (Input::GetKey(KEY::S))		pos.z += 15;
		if (Input::GetKey(KEY::A))		pos.x -= 15;
		if (Input::GetKey(KEY::D))		pos.x += 15;

		pos.x += ControllerInput::GetLXRaw(CONUSER::TWO) * 15;
		pos.z -= ControllerInput::GetLYRaw(CONUSER::TWO) * 15;

		if ((Input::GetKey(KEY::LSHIFT) || ControllerInput::GetButton(BUTTON::B, CONUSER::TWO)) && pos.y < 500) {
			pos.y -= 15 * m_dt;
			//ballPhys.SetVelocity(btVector3(0.f, 0.f, 0.f));
		}
		if (Input::GetKeyDown(KEY::SPACE) || ControllerInput::GetButtonDown(BUTTON::A, CONUSER::TWO)) {
			pos.y = 35 /*x at least two times this*/;
			//pos.y += 15;
		}
		if (pos.x != 0 || pos.z != 0) {
			//if (!ECS::GetComponent<ObjMorphLoader>(bodyEnt).IsAnim("char/walk"))
			ECS::GetComponent<ObjMorphLoader>(bodyEntoff2).BlendTo("char/walk");
		}
		else {
			//if (!ECS::GetComponent<ObjMorphLoader>(bodyEnt).IsAnim("char/idle"))
			ECS::GetComponent<ObjMorphLoader>(bodyEntoff2).BlendTo("char/idle");
		}
		pos = glm::vec4(pos, 1) * glm::rotate(glm::mat4(1.f), rot2.y, glm::vec3(0, 1, 0));
		ballPhys2.SetVelocity(pos);
	}


	if (Input::GetKey(KEY::P)) {
		if (Input::GetKey(KEY::LSHIFT))
			Rendering::LightColour[0] = glm::vec3(3.f);
		else
			Rendering::LightColour[0] = glm::vec3((rand() % 30) / 10.f, (rand() % 30) / 10.f, (rand() % 30) / 10.f);
	}

	/// End of loop
	if (Input::GetKeyDown(KEY::FSLASH))	m_colliders.ToggleDraw();
	m_colliders.Update(m_dt);
	if (Input::GetKeyDown(KEY::F10))	if (!m_colliders.SaveToFile(false))	std::cout << "file save failed\n";
	if (Input::GetKeyDown(KEY::F1))		if (!m_colliders.LoadFromFile())	std::cout << "file load failed\n";

	{
		///
		///RAYCAST TEST
		///
		auto& ptrans = ECS::GetComponent<Transform>(P2);
		glm::vec3 rayPos = ptrans.ComputeScalessGlobal().GetGlobalPosition();
		glm::vec3 forwards = -ptrans.GetForwards();
		btVector3 p = PhysBody::GetRaycast(rayPos, forwards * 2000.f);
		if (p != btVector3())
		{
			if (Input::GetKey(KEY::RSHIFT) || ControllerInput::GetButton(BUTTON::X, CONUSER::TWO)) {

				ShootLazer(glm::length(BLM::BTtoGLM(p) - rayPos), ptrans.GetGlobalRotation(), rayPos, false);
			}
		}
		RayResult test = PhysBody::GetRaycastResult(BLM::GLMtoBT(rayPos), BLM::GLMtoBT(-forwards * camDistance * 10.f));
		//RayResult test = PhysBody::GetRaycastResult(BLM::GLMtoBT(rayPos), BLM::GLMtoBT(-forwards * camDistance));
		if (rot2.x < pi * 0.25f) {
			ECS::GetComponent<Transform>(cameraEnt2).SetPosition(glm::vec3(
				0, 0, (test.hasHit() ? (test.m_closestHitFraction >= 0.1f ?
					camDistance : test.m_closestHitFraction * camDistance * 10.f - 0.5f) : camDistance)
			));
		}
		else if (rot2.x < pi * 0.5f) {
			float t = (rot2.x / pi - 0.25f) * 4;
			ECS::GetComponent<Transform>(cameraEnt2).SetPosition(glm::vec3(
				0, 0, (1 - t) * camDistance - t * 0.5f
			));
		}
		else {
			ECS::GetComponent<Transform>(cameraEnt2).SetPosition(glm::vec3(
				0, 0, 0.5f
			));
		}
	}

	{
		///
		///RAYCAST TEST
		///
		auto& ptrans = ECS::GetComponent<Transform>(P);
		glm::vec3 rayPos = ptrans.ComputeScalessGlobal().GetGlobalPosition();
		glm::vec3 forwards = -ptrans.GetForwards();
		btVector3 p = PhysBody::GetRaycast(rayPos, forwards * 2000.f);
		if (p != btVector3())
		{
			//Rendering::LightPos = ECS::GetComponent<Transform>(epic).SetPosition(p).GetGlobalPosition();

			if (Input::GetKey(KEY::RSHIFT) || ControllerInput::GetButton(BUTTON::X, CONUSER::ONE)) {

				ShootLazer(glm::length(BLM::BTtoGLM(p) - rayPos), ptrans.GetGlobalRotation(), rayPos, true);
			}
		}
		RayResult test = PhysBody::GetRaycastResult(BLM::GLMtoBT(rayPos), BLM::GLMtoBT(-forwards * camDistance * 10.f));
		//RayResult test = PhysBody::GetRaycastResult(BLM::GLMtoBT(rayPos), BLM::GLMtoBT(-forwards * camDistance));
		if (rot.x < pi * 0.25f) {
			ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(
				0, 0, (test.hasHit() ? (test.m_closestHitFraction >= 0.1f ?
					camDistance : test.m_closestHitFraction * camDistance * 10.f - 0.5f) : camDistance)
			));
		}
		else if (rot.x < pi * 0.5f) {
			float t = (rot.x / pi - 0.25f) * 4;
			ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(
				0, 0, (1 - t) * camDistance - t * 0.5f
			));
		}
		else {
			ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(
				0, 0, 0.5f
			));
		}
	}


	updateLazer();
}

void DemoScene::Exit()
{
	if (!m_colliders.SaveToFile(false))
		std::cout << "file save failed\n";
}

void DemoScene::ShootLazer(float width, glm::quat rotation, glm::vec3 pos, bool isp1)
{
	if (isp1) {
		if (Lazer == -1) {
			Lazer = ECS::CreateEntity();
			ECS::AttachComponent<ObjMorphLoader>(Lazer).LoadMeshs("effects/laser", true);
			ECS::GetComponent<Transform>(Lazer).SetRotation(rotation).SetPosition(pos).SetScale(glm::vec3(1, 1, width));
			ouch.play();
		}
	}
	else {
		if (Lazer2 == -1) {
			Lazer2 = ECS::CreateEntity();
			ECS::AttachComponent<ObjMorphLoader>(Lazer2).LoadMeshs("effects/laser", true);
			ECS::GetComponent<Transform>(Lazer2).SetRotation(rotation).SetPosition(pos).SetScale(glm::vec3(1, 1, width));
			ouch.play();
		}
	}
}

void DemoScene::updateLazer()
{
	if (Lazer != -1) {
		if (ECS::GetComponent<ObjMorphLoader>(Lazer).IsDone()) {
			ECS::DestroyEntity(Lazer);
			Lazer = -1;
		}
	}
	if (Lazer2 != -1) {
		if (ECS::GetComponent<ObjMorphLoader>(Lazer2).IsDone()) {
			ECS::DestroyEntity(Lazer2);
			Lazer2 = -1;
		}
	}
}
