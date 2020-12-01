#include "Utilities/Gameloop.h"
#include "DemoScene.h"
#include "Tutorial.h"

int main() {
	int width = 1280, height = 720;
	GLFWwindow* window = Gameloop::Start("Mecha Mayhem", width, height);
	if (!window)	return 1;

	{
		// Creating demo scene
		std::vector<Scene*> scenes;
		scenes.push_back(new Tutorial("Tutorial", glm::vec3(0, -100, 0)));
		scenes.push_back(new DemoScene("Demo 2", glm::vec3(0, -100, 0)));
		 
		scenes[0]->Init(width, height);
		scenes[1]->Init(width, height);

		bool sceneswap = false;
		Scene* activeScene = scenes[sceneswap]->Reattach();
		glfwSetWindowTitle(window, activeScene->GetName().c_str());

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			//update the static dt
			Time::Update(glfwGetTime());

			if (BackEnd::LostFocus())	continue;

			ControllerInput::ControllerRefresh();

			if (Input::GetKey(KEY::LCTRL) && Input::GetKeyUp(KEY::ENTER)) {
				activeScene->Exit();
				glfwSetWindowTitle(window, (activeScene = scenes[sceneswap = !sceneswap]->Reattach())->GetName().c_str());
			}

			activeScene->Update();

			//do not touch plz
			activeScene->BackEndUpdate();
			Gameloop::Update();

			glfwSwapBuffers(window);
		}

		activeScene->Exit();
	}

	Gameloop::Stop();
	return 0;
}