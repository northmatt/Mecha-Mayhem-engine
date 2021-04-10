#include "Utilities/Gameloop.h"
#include "DemoScene.h"
#include "Tutorial.h"
#include "MainMenu.h"
#include "LeaderBoard.h"
//#include "MapEditorScene.h"

int main() {
	int width = 1280, height = 720;
	bool showImGui = false;
	const bool usingImGui = false;
	Logger::outputLogger = false;
	
	GLFWwindow* window = Gameloop::Start("Mecha Mayhem", width, height, usingImGui);
	if (!window)	return 1;

	{
		//set audio levels
		//AudioEngine::Instance().GetBus("Music").SetVolume(0.25f);

		//this has to be done here
		DemoScene::gameEndCube.loadFromFile("win.cube");

		// Creating demo scenes
		Scene::m_scenes.push_back(new MainMenu("Mecha Mayhem"));
		Scene::m_scenes.push_back(new Tutorial("MM Tutorial", glm::vec3(0, -100, 0)));
		Scene::m_scenes.push_back(new DemoScene("MM Demo", glm::vec3(0, -100, 0)));
		Scene::m_scenes.push_back(new LeaderBoard("The Winner is..."));

		Scene::m_scenes[0]->Init(width, height);
		Scene::m_scenes[1]->Init(width, height);
		Scene::m_scenes[2]->Init(width, height);
		Scene::m_scenes[3]->Init(width, height);

		Scene::m_activeScene = Scene::m_scenes[0]->Reattach();
		/*
		//controller	model	score	colour
		LeaderBoard::players[0] = { CONUSER::NONE,	1,		0,		glm::vec3(0,0,0) };
		LeaderBoard::players[1] = { CONUSER::NONE,	2,		0,		glm::vec3(0,0,0) };
		LeaderBoard::players[2] = { CONUSER::NONE,	3,		0,		glm::vec3(0,0,0) };
		LeaderBoard::players[3] = { CONUSER::NONE,	4,		0,		glm::vec3(0,0,0) };

		Scene::m_scenes.push_back(new MapEditor("uh, not for playing"));
		Scene::m_scenes[4]->Init(width, height);

		Scene::m_activeScene->Exit();

		Scene::m_activeScene = Scene::m_scenes[4]->Reattach();//*/
		/*
		LeaderBoard::players[0] = { CONUSER::ONE,	1,		0,		glm::vec3(0,0,0) };
		LeaderBoard::players[1] = { CONUSER::ONE,	2,		0,		glm::vec3(0,0,0) };
		LeaderBoard::players[2] = { CONUSER::ONE,	3,		0,		glm::vec3(0,0,0) };
		LeaderBoard::players[3] = { CONUSER::ONE,	4,		0,		glm::vec3(0,0,0) };
		LeaderBoard::playerCount = 4;
		LeaderBoard::scoreGoal = 0;
		Scene::m_activeScene = Scene::m_scenes[2]->Reattach();//*/
		glfwSetWindowTitle(window, Scene::m_activeScene->GetName().c_str());
		
		bool paused = false;


		//start teh game in fullscreen
		BackEnd::SetFullscreen();


		//called after init because init takes time, which breaks intro anim
		Time::Update(glfwGetTime());
		Time::dt = 0;

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			//std::cout << 1.f / Time::dt << "              \r";
			if (Scene::GetExit()) {
				break;
			}

			if (!BackEnd::HasFocus()) {
				if (!paused) {
					AudioEngine::Instance().PauseEverything();
					SoundEventManager::Update();
					paused = true;
				}
				//has to update all the time
				Time::Update(glfwGetTime());
				if (usingImGui)
					Gameloop::ImGuiWindow(window, Scene::m_activeScene);
				continue;
			}
			if (paused) {
				AudioEngine::Instance().UnPauseEverything();
				paused = false;
			}

			ControllerInput::ControllerRefresh();

			//delete this?
			if (Input::GetKeyDown(KEY::F)) {
				if (BackEnd::GetFullscreen())	BackEnd::SetTabbed();
				else							BackEnd::SetFullscreen();
			}
			if (Input::GetKeyDown(KEY::F1))		showImGui = !showImGui;
			//delete this?

			Scene::m_activeScene->Update();
			Scene::doSceneChange(window);
			
			//do not touch plz
			Scene::m_activeScene->BackEndUpdate();
			if (usingImGui && showImGui)
				Gameloop::ImGuiWindow(window, Scene::m_activeScene);
			Gameloop::Update();

			glfwSwapBuffers(window);
		}

		Scene::m_activeScene->Exit();

		//this has to be done here
		DemoScene::gameEndCube.Unload();
	}

	Gameloop::Stop(usingImGui);
	return 0;
}