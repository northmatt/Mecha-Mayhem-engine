#include "Engine/Rendering.h"
#include "bullet/btBulletDynamicsCommon.h"

GLFWwindow* window;

int main() {
	srand(time(0));
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	int width = 1280, height = 720;

	if (!(window = BackEnd::Init("Mecha Mayhem", width, height)))	return 1;

	ObjLoader::Init();

	entt::registry reg;

	ECS::AttachRegistry(&reg);
	
	//1
	btDbvtBroadphase* _broadphase = new btDbvtBroadphase();

	//2
	btCollisionConfiguration* _collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* _dispatcher = new btCollisionDispatcher(_collisionConfiguration);

	//3
	btSequentialImpulseConstraintSolver* _solver = new btSequentialImpulseConstraintSolver();

	//4
	btDiscreteDynamicsWorld* _world = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);

	//5
	_world->setGravity(btVector3(0, -9.8, 0));


	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(1.), btScalar(50.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		_world->addRigidBody(body);
	}

		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(2, 100, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setRestitution(1);

		_world->addRigidBody(body);

	/// Creating Entities

	unsigned cameraEnt = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
	camCam.SetFovDegrees(60.f).SetAspect(float(width) / height);
	
	unsigned Dio = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio).LoadMesh("models/Char.obj", true);
	ECS::GetComponent<Transform>(Dio).SetPosition(glm::vec3(0, 30, 0));

	unsigned Dio2 = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio2).LoadMesh("models/Char.obj", true);
	ECS::GetComponent<Transform>(Dio2).SetPosition(glm::vec3(0, 10, 0)).ChildTo(Dio);

	unsigned Dio3 = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio3).LoadMesh("models/Pistol.obj", true);
	ECS::GetComponent<Transform>(Dio3).SetPosition(glm::vec3(0, -20, 0)).ChildTo(Dio2);
	{
		unsigned entity = ECS::CreateEntity();
		auto& temp = ECS::AttachComponent<ObjLoader>(entity).LoadMesh("models/cringe.obj", true);
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -20, 0));
	}

	std::vector<unsigned> someObjs = {};

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

	auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);

	/// End of creating entities

	float lastClock = glfwGetTime();

	constexpr float pi = glm::half_pi<float>() - 0.01f;
	glm::quat startQuat = glm::rotation(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	bool change = true;
	glm::vec2 rot = glm::vec2(0.f);

	bool buttonPressed = false;
	bool screen = true;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float deltaTime = glfwGetTime() - lastClock;
		lastClock = glfwGetTime();
		std::cout << 1.f / deltaTime << '\r';

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
			rot.x += 3.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			rot.x -= 3.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			rot.y -= 3.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			rot.y += 3.f * deltaTime;
			change = true;
		}
		if (rot.x > pi) {
			rot.x = pi;
		}
		else if (rot.x < -pi) {
			rot.x = -pi;
		}
		if (change) {
			glm::quat rotf = glm::rotate(startQuat, rot.x, glm::vec3(1, 0, 0));
			rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
			camTrans.SetRotation(rotf);
			change = false;
		}
		/*glm::mat4 rotf = glm::rotate(glm::mat4(1.f), rot.x, glm::vec3(1, 0, 0));
		rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
		camTrans.SetRotation(glm::mat3(rotf));*/

		glm::vec3 pos2 = glm::vec3(0.f);
		if (glfwGetKey(window, GLFW_KEY_I)) {
			pos2.z += 15 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_K)) {
			pos2.z -= 15 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_J)) {
			pos2.x += 15 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_L)) {
			pos2.x -= 15 * deltaTime;
		}
		ECS::GetComponent<Transform>(Dio).SetPosition(
			ECS::GetComponent<Transform>(Dio).GetPosition() + pos2);
		ECS::GetComponent<Transform>(Dio2).SetPosition(
			ECS::GetComponent<Transform>(Dio2).GetPosition() + pos2);
		ECS::GetComponent<Transform>(Dio3).SetPosition(
			ECS::GetComponent<Transform>(Dio3).GetPosition() + pos2);

		glm::vec3 pos = glm::vec3(0.f);
		if (glfwGetKey(window, GLFW_KEY_W)) {
			pos.z += 15 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			pos.z -= 15 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			pos.x += 15 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			pos.x -= 15 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
			pos.y -= 15 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			pos.y += 15 * deltaTime;
		}
		if (pos.x != 0 || pos.y != 0 || pos.z != 0) {
			pos = glm::vec4(pos, 1) * glm::rotate(glm::mat4(1.f), rot.y, glm::vec3(0, 1, 0));
			camTrans.SetPosition(camTrans.GetPosition() + pos);
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


		{
			btTransform trans = body->getWorldTransform();
			ECS::GetComponent<Transform>(Dio).SetPosition(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
		}

		if (glfwGetKey(window, GLFW_KEY_0)) {
			body->setLinearVelocity(btVector3(0, 10, 0));
		}


		_world->stepSimulation(deltaTime, 10);

		//print positions of all objects
		for (int j = _world->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = _world->getCollisionObjectArray()[j];
			btRigidBody* bodyt = btRigidBody::upcast(obj);
			btTransform trans;
			if (bodyt && bodyt->getMotionState())
			{
				bodyt->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = obj->getWorldTransform();
			}
		}








		/// End of loop

		Rendering::Update(&reg);

		glfwSwapBuffers(window);
	}

	Shader::UnBind();
	VertexArrayObject::UnBind();


	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
}