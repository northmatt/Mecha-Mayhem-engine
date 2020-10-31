#include "BackEndThings/Rendering.h"

GLFWwindow* window;

int main() {
	srand(time(0));
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	if (!(window = BackEnd::Init("Mecha Mayhem")))	return 1;

	int width = 720, height = 480;

	glfwSetWindowSize(window, width, height);
	//glfwGetWindowSize(window, &width, &height);
	//if (height + width == 0)	return 1;

	ObjLoader::Init();

	entt::registry reg;

	ECS::AttachRegistry(&reg);
	
	/// Creating Entities

	unsigned cameraEnt = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
	camCam.SetFovDegrees(60.f);
	
	unsigned Dio = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(Dio).LoadMesh("models/Char.obj", true);
	ECS::GetComponent<Transform>(Dio).SetPosition(glm::vec3(0, 30, 0));


	std::vector<unsigned> someObjs = {};

	unsigned amt = 100;

	for (int count(0); count < amt; ++count) {
		someObjs.push_back(ECS::CreateEntity());
		ECS::AttachComponent<ObjLoader>(someObjs[count]).LoadMesh("models/blade.obj", count % 2);

		auto& trans = ECS::GetComponent<Transform>(someObjs[count]);

		glm::vec3 axis = glm::vec3(rand() % 2, rand() % 2, rand() % 2 + 1);
		trans.SetPosition(glm::vec3(rand() % 21 - 10, rand() % 21 - 10, rand() % 21 - 10))
			->SetScale(glm::vec3((rand() % 8 + 3) / 10.f))
			->SetRotation(glm::rotate(glm::quat(1.f, 0, 0, 0), float(rand() % 8 - 3), axis));
	}

	auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);

	/// End of creating entities

	float lastClock = glfwGetTime();

	constexpr float pi = glm::half_pi<float>() - 0.01f;
	glm::quat startQuat = glm::rotation(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	bool change = true;
	glm::vec2 rot = glm::vec2(0.f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float deltaTime = glfwGetTime() - lastClock;
		lastClock = glfwGetTime();

		/// Start of loop
		glfwGetWindowSize(window, &width, &height);
		camCam.SetAspect(float(width) / height);


		if (glfwGetKey(window, GLFW_KEY_UP)) {
			rot.x += 2.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			rot.x -= 2.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			rot.y -= 2.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			rot.y += 2.f * deltaTime;
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
			pos2.z += 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_K)) {
			pos2.z -= 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_J)) {
			pos2.x += 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_L)) {
			pos2.x -= 5 * deltaTime;
		}
		ECS::GetComponent<Transform>(Dio).SetPosition(
			ECS::GetComponent<Transform>(Dio).GetPosition() + pos2);

		glm::vec3 pos = glm::vec3(0.f);
		if (glfwGetKey(window, GLFW_KEY_W)) {
			pos.z += 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			pos.z -= 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			pos.x += 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			pos.x -= 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
			pos.y -= 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			pos.y += 5 * deltaTime;
		}
		if (pos.x != 0 || pos.y != 0 || pos.z != 0) {
			pos = glm::vec4(pos, 1) * glm::rotate(glm::mat4(1.f), rot.y, glm::vec3(0, 1, 0));
			camTrans.SetPosition(camTrans.GetPosition() + pos);
		}


		if (glfwGetKey(window, GLFW_KEY_P)) {
			for (int count(0); count < someObjs.size(); ++count) {
				ECS::GetComponent<Transform>(someObjs[count]).SetPosition(
					glm::vec3(rand() % 21 - 10, rand() % 21 - 10, rand() % 21 - 10))
					->SetScale(glm::vec3((rand() % 8 + 3) / 10.f));
			}
			Rendering::DefaultColour = glm::vec3((rand() % 10) / 10.f, (rand() % 10) / 10.f, (rand() % 10) / 10.f);
			Rendering::LightColour = glm::vec3((rand() % 30) / 10.f, (rand() % 30) / 10.f, (rand() % 30) / 10.f);
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
