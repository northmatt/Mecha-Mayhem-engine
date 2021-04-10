#pragma once
#include "Rendering.h"
#include "Time.h"
#include "Input.h"
#include "SoundEventManager.h"

//post effect stuff
#include "Effects/Post/BloomEffect.h"
#include "Effects/Post/PixelEffect.h"
#include "Effects/Post/SepiaEffect.h"
#include "Effects/Post/GreyscaleEffect.h"
#include "Effects/Post/DepthOfField.h"
#include "Effects/Post/ColourCorrectionEffect.h"

class Scene
{
public:
	Scene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f), bool physics = true);
	~Scene();

	entt::registry* GetRegistry();
	std::string GetName();

	virtual void Init(int windowWidth, int windowHeight);

	//called on scene change
	virtual Scene* Reattach();

	//everyframe
	virtual void Update() {}
	//after physics update, mainly for lights and stuff
	virtual void LateUpdate() {}

	//drawing pauseScrenes and stuff
	virtual void DrawOverlay() {
		//Gbuffer fixes this
		//glViewport(0, 0, BackEnd::GetWidth(), BackEnd::GetHeight());
	}

	//called on scene change
	virtual void Exit() {
		//reset sound stuff
		SoundEventManager::ResetEvents();
	}

	//put ImGui stuff here
	virtual void ImGuiFunc();

	//can't override
	virtual void BackEndUpdate() final;
	static bool GetExit() {
		return m_exitGame;
	}

	static void QueueSceneChange(size_t index);
	static void doSceneChange(GLFWwindow* window);

	static void UnloadScenes();

	static std::vector<Scene*> m_scenes;
	static Scene* m_activeScene;

protected:
	std::string m_name;
	bool m_paused = false;
	short m_camCount = 1;
	int maxEffectCount = 1;

	static size_t m_nextScene;
	static bool m_doSceneChange;
	static bool m_exitGame;

	entt::registry m_reg;
	btDiscreteDynamicsWorld *m_world = nullptr;
	HitboxGen m_colliders;
	Effects m_effects;
	FrameEffects m_frameEffects;

	btDbvtBroadphase *_broadphase = nullptr;
	btCollisionConfiguration* _collisionConfiguration = nullptr;
	btCollisionDispatcher* _dispatcher = nullptr;
	btSequentialImpulseConstraintSolver* _solver = nullptr;
};