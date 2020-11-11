#include "Utilities/Gameloop.h"
#include "DemoScene.h"

int main() {
	int width = 1280, height = 720;
	GLFWwindow* window = Gameloop::Start("Mecha Mayhem", width, height);
	if (!window)	return 1;

	{
		// Creating demo scene
		std::vector<Scene*> scenes;
		scenes.push_back(new DemoScene("Demo", glm::vec3(0, -50, 0)));

		Scene* activeScene = scenes[0];

		activeScene->Init(width, height);
		glfwSetWindowTitle(window, activeScene->GetName().c_str());

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();


			activeScene->Update();


			//do not touch plz
			activeScene->BackEndUpdate();
			Gameloop::Update();

			glfwSwapBuffers(window);
		}
	}

	Gameloop::Stop();
	return 0;
}