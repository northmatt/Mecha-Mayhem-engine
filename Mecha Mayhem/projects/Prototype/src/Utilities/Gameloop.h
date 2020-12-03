#pragma once
#include "Scene.h"

//holds all inits and unloads
namespace Gameloop
{
	//init everything and return the window from backend
	GLFWwindow* Start(const std::string& name, int width, int height) {
		srand(time(0));
		// We'll borrow the logger from the toolkit, but we need to initialize it
		Logger::Init();
		SoundManager::init("./sounds/", 50);

		GLFWwindow* window = BackEnd::Init("Mecha Mayhem", width, height);
		if (!window)	return nullptr;

		//all static inits
		ObjLoader::Init();
		ObjMorphLoader::Init();
		Sprite::Init();

		Input::Init(window);
		HitboxGen::Init();
		Effects::Init();
		Player::Init(width, height);

		return window;
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
	void Stop() {
		//unloading static things
		Input::Unload();
		PhysBody::Unload();
		ECS::DettachRegistry();
		BackEnd::Unload();
		ObjLoader::Unload();
		ObjMorphLoader::Unload();
		Sprite::Unload();

		SoundManager::stopEverything();

		// Clean up the toolkit logger so we don't leak memory
		Logger::Uninitialize();
	}
}