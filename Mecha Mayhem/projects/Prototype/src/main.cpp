#include "Utilities/Gameloop.h"
#include "DemoScene.h"
#include "Tutorial.h"
#include "MainMenu.h"
#include "LeaderBoard.h" 

int main() {
	int width = 1280, height = 720;
	const bool usingImGui = false;

	GLFWwindow* window = Gameloop::Start("Mecha Mayhem", width, height, usingImGui);
	if (!window)	return 1;

	{
		// Creating demo scenes
		std::vector<Scene*> scenes;
		scenes.push_back(new MainMenu("Mecha Mayhem"));
		scenes.push_back(new Tutorial("MM Tutorial", glm::vec3(0, -100, 0))); 
		scenes.push_back(new DemoScene("MM Demo", glm::vec3(0, -100, 0)));
		scenes.push_back(new LeaderBoard("THe Winner is..."));
 		 
		scenes[0]->Init(width, height);
		scenes[1]->Init(width, height);
		scenes[2]->Init(width, height);
		scenes[3]->Init(width, height);

		int currScene = 0, count = scenes.size();
		Scene* activeScene = scenes[0]->Reattach();
		glfwSetWindowTitle(window, activeScene->GetName().c_str());

		bool paused = false;

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();

			if (activeScene->ExitTest()) {
				break;
			}

			if (!BackEnd::HasFocus()) {
				if (!paused) {
					SoundManager::PauseEverything();
					paused = true;
				}
				continue;
			}
			else if (paused) {
				SoundManager::PlayEverything();
				paused = false;
			}

			ControllerInput::ControllerRefresh();

			activeScene->Update();

			if ((currScene = activeScene->ChangeScene(count)) > -1) {
				activeScene->Exit();
				glfwSetWindowTitle(window, (activeScene = scenes[currScene]->Reattach())->GetName().c_str());
			}

			//do not touch plz
			activeScene->BackEndUpdate();
			Gameloop::Update();
			if (usingImGui)
				Gameloop::ImGuiWindow(window, activeScene);

			glfwSwapBuffers(window);
		}

		activeScene->Exit();
	}

	Gameloop::Stop(usingImGui);
	return 0;
}