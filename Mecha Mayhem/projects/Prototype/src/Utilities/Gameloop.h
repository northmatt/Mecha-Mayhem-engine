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
		GLFWwindow* window = BackEnd::Init("Mecha Mayhem", width, height);
		if (!window)	return nullptr;

		AudioEngine& engine = AudioEngine::Instance();
		engine.Init();
		engine.LoadBankWithString("Master");
		engine.LoadBus("Music", "bus:/Music");
		engine.LoadBus("SFX", "bus:/SFX");
		engine.CreateSoundEvent("MainMenu", "event:/Main Menu");
		engine.CreateSoundEvent("hit", "event:/punch");
		engine.CreateSoundEvent("reload", "event:/reload");
		engine.CreateSoundEvent("step", "event:/steps");
		engine.CreateSoundEvent("shoot", "event:/Pew");
		engine.CreateSoundEvent("pickup", "event:/pickup");

		engine.GetListener().SetPosition(glm::vec3(0, 0, 0));

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
		Framebuffer::InitFullscreenQuad();
		FrameEffects::Init();

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
		//Controller Checks n stuff
		ControllerInput::ControllerUpdate();
		//this
		AudioEngine::Instance().Update();
		//updated in Scene.cpp
		//Player::Update();
		//update the static dt
		Time::Update(glfwGetTime());
	}

	//stop everything (use at the end to avoid issues)
	void Stop(bool usingImGui) {
		//unloading static things
		Input::Unload();
		PhysBody::Unload();
		ECS::DettachRegistry();
		Player::Unload();
		ObjLoader::Unload();
		MultiTextObj::Unload();
		ObjMorphLoader::Unload();
		Sprite::Unload();
		Effects::Unload();
		FrameEffects::Unload();

		Scene::UnloadScenes();

		if (usingImGui)	BackEnd::CloseImGui();
		BackEnd::Unload();

		AudioEngine::Instance().Shutdown();

		// Clean up the toolkit logger so we don't leak memory
		Logger::Uninitialize();
	}
}