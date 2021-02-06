#pragma once
#include "Scene.h"

//holds all inits and unloads
namespace Gameloop
{
	//init everything and return the window from backend
	GLFWwindow* Start(const std::string& name, int width, int height, bool usingImGui) {
		srand(time(0));
		// We'll borrow the logger from the toolkit, but we need to initialize it
		Logger::Init();
		SoundManager::init("./sounds/", 50);

		GLFWwindow* window = BackEnd::Init("Mecha Mayhem", width, height);
		if (!window)	return nullptr;

		if (usingImGui)	BackEnd::InitImGui();

		//all static inits
		ObjLoader::Init();
		ObjMorphLoader::Init();
		Sprite::Init();

		Input::Init(window);
		HitboxGen::Init();
		Effects::Init();
		Player::Init(width, height);
		Rendering::Init(width, height);

		return window;
	}

	void ImGuiWindow(GLFWwindow* window, Scene* currScene)
	{
		// Implementation new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		// ImGui context new frame
		ImGui::NewFrame();

		if (ImGui::Begin(currScene->GetName().c_str())) {
			// Render our GUI stuff
			currScene->ImGuiFunc();
		}
		ImGui::End();

		// Make sure ImGui knows how big our window is
		ImGuiIO& io = ImGui::GetIO();
		int width{ 0 }, height{ 0 };
		glfwGetWindowSize(window, &width, &height);
		io.DisplaySize = ImVec2((float)width, (float)height);

		// Render all of our ImGui elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// If we have multiple viewports enabled (can drag into a new window)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			// Update the windows that ImGui is using
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			// Restore our gl context
			glfwMakeContextCurrent(window);
		}
	}

	//update anything with global updates
	void Update() {
		//Keybored
		Input::Update();
		//lowercase lol, sound stuff yea
		SoundManager::update();
		//Controller Checks n stuff
		ControllerInput::ControllerUpdate();
		//update the static dt
		Time::Update(glfwGetTime());

		Player::Update();
	}

	//stop everything (use at the end to avoid issues)
	void Stop(bool usingImGui) {
		//unloading static things
		Input::Unload();
		PhysBody::Unload();
		ECS::DettachRegistry();
		BackEnd::Unload();
		ObjLoader::Unload();
		ObjMorphLoader::Unload();
		Sprite::Unload();
		Effects::Unload();

		if (usingImGui)	BackEnd::CloseImGui();

		SoundManager::stopEverything();

		// Clean up the toolkit logger so we don't leak memory
		Logger::Uninitialize();
	}
}