#pragma once
#include "Scene.h"

//holds all inits and unloads
namespace Gameloop
{
	GLFWwindow* Start(const std::string& name, int width, int height) {
		srand(time(0));
		// We'll borrow the logger from the toolkit, but we need to initialize it
		Logger::Init();

		GLFWwindow* window = BackEnd::Init("Mecha Mayhem", width, height);
		if (!window)	return nullptr;

		//all static inits
		ObjLoader::Init();
		Scene::Init();

		return window;
	}

	void Stop() {
		//unloading static things
		PhysBody::Unload();
		ECS::DettachRegistry();
		BackEnd::Unload();
		ObjLoader::Unload();
		Scene::Unload();

		// Clean up the toolkit logger so we don't leak memory
		Logger::Uninitialize();
	}
}