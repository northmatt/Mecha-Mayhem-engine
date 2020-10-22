#include "BackEndThings/Rendering.h"

GLFWwindow* window;

int main() {
	srand(time(0));
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	if (!(window = BackEnd::Init("Mecha Mayhem")))	return 1;

	ObjLoader::Init();

	entt::registry reg;

	ECS::AttachRegistry(&reg);
	
	unsigned cameraEnt = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt);

	std::vector<unsigned> someObjs = {};

	unsigned amt = 1000;

	for (int count(0); count < amt; ++count) {
		someObjs.push_back(ECS::CreateEntity());
		ECS::AttachComponent<ObjLoader>(someObjs[count], ObjLoader("blade.obj", count % 2));

		auto& trans = ECS::GetComponent<Transform>(someObjs[count]);

		trans.SetPosition(glm::vec3(rand() % 20 - 10, rand() % 20 - 10, rand() % 20 - 10));
		trans.SetScale(glm::vec3((rand() % 8 + 3) / 10.f));
		glm::vec3 axis = glm::vec3(rand() % 2, rand() % 2, rand() % 2 + 1);
		trans.SetRotation(glm::rotate(glm::quat(1.f, 0, 0, 0), float(rand() % 7 - 3), axis));
	}

	auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);

	float lastClock = glfwGetTime();

	glm::vec2 rot = glm::vec2(0.f);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float deltaTime = glfwGetTime() - lastClock;
		lastClock = glfwGetTime();

		if (glfwGetKey(window, GLFW_KEY_UP)) {
			rot.x += 2.f * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			rot.x -= 2.f * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			rot.y -= 2.f * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			rot.y += 2.f * deltaTime;
		}
		float pi = glm::half_pi<float>() - 0.01f;
		if (rot.x > pi) {
			rot.x = pi;
		}
		else if (rot.x < -pi) {
			rot.x = -pi;
		}
		glm::quat rotf = glm::rotate(glm::quat(1.f, 0.f, 0.f, 0.f), rot.x, glm::vec3(1, 0, 0));
		rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
		camTrans.SetRotation(rotf);
		/*glm::mat4 rotf = glm::rotate(glm::mat4(1.f), rot.x, glm::vec3(1, 0, 0));
		rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
		camTrans.SetRotation(glm::mat3(rotf));*/

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
		pos = glm::vec4(pos, 1) * glm::rotate(glm::mat4(1.f), rot.y, glm::vec3(0, 1, 0));
		camTrans.SetPosition(camTrans.GetPosition() + pos);


		if (glfwGetKey(window, GLFW_KEY_P)) {
			for (int count(0); count < amt; ++count) {
				ECS::GetComponent<Transform>(someObjs[count]).SetPosition(
					glm::vec3(rand() % 20 - 10, rand() % 20 - 10, rand() % 20 - 10));
				ECS::GetComponent<Transform>(someObjs[count]).SetScale(
					glm::vec3((rand() % 8 + 3) / 10.f));
			}
		}

		Rendering::Update(&reg);

		glfwSwapBuffers(window);
	}

	Shader::UnBind();
	VertexArrayObject::UnBind();


	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
} 
