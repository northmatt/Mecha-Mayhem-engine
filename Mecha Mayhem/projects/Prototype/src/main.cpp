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
		scenes.push_back(new DemoScene("Demo 2", glm::vec3(0, -50, 0)));

		scenes[0]->Init(width, height);
		scenes[1]->Init(width, height);

		bool sceneswap = false;
		Scene* activeScene = scenes[sceneswap]->Reattach();
		glfwSetWindowTitle(window, activeScene->GetName().c_str());

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();

			if (Input::GetKey(KEY::LCTRL) && Input::GetKeyUp(KEY::ENTER)) {
				glfwSetWindowTitle(window,
					(activeScene = scenes[sceneswap = !sceneswap]->Reattach())->
					GetName().c_str()
				);
			}

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