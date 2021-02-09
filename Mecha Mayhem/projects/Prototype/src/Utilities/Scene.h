#pragma once
#include "Rendering.h"
#include "Time.h"
#include "Input.h"
#include <SoundSystem.h>

class Scene
{
public:
	Scene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f), bool physics = true);
	~Scene();

	entt::registry* GetRegistry();
	std::string GetName();

	virtual void Init(int windowWidth, int windowHeight);

	virtual Scene* Reattach();

	virtual void Update() {}
	virtual void Exit() {}

	virtual void ImGuiFunc() {
		if (ImGui::Button("Ambient Toggle", ImVec2(140.0f, 20.0f))) {
			doAmbient = !doAmbient;
		}
		
		if (ImGui::Button("Diffuse Toggle", ImVec2(140.0f, 20.0f))) {
			doDiffuse = !doDiffuse;
		}
		
		if (ImGui::Button("Spec Toggle", ImVec2(140.0f, 20.0f))) {
			doSpecular = !doSpecular;
		}

		if (ImGui::Button("Multi Light Toggle", ImVec2(140.0f, 20.0f))) {
			doMultiLight = !doMultiLight;
		}

		if (ImGui::Button("Diffuse Ramp Toggle", ImVec2(140.0f, 20.0f))) {
			doDiffuseRamp = !doDiffuseRamp;
		}

		if (ImGui::Button("Spec Ramp Toggle", ImVec2(140.0f, 20.0f))) {
			doSpecularRamp = !doSpecularRamp;
		}

		if (ImGui::Button("Warm LUT Toggle", ImVec2(140.0f, 20.0f))) {
			doWarmCol = !doWarmCol;
		}

		if (ImGui::Button("Cold LUT Toggle", ImVec2(140.0f, 20.0f))) {
			doColdCol = !doColdCol;
		}

		if (ImGui::Button("Custom LUT Toggle", ImVec2(140.0f, 20.0f))) {
			doCustomCol = !doCustomCol;
		}
	};

	virtual void BackEndUpdate();
	virtual int ChangeScene(int sceneCount) final;
	virtual bool ExitTest() final {
		return m_exitGame;
	}

	static Scene* _activeScene;

protected:
	std::string m_name;
	short m_camCount = 1;
	int m_nextScene = -1;
	bool m_paused = false;
	bool m_exitGame = false;

	bool doAmbient = true;
	bool doDiffuse = true;
	bool doSpecular = true;
	bool doMultiLight = true;
	bool doDiffuseRamp = false;
	bool doSpecularRamp = false;
	bool doWarmCol = false;
	bool doColdCol = false;
	bool doCustomCol = false;

	entt::registry m_reg;
	btDiscreteDynamicsWorld *m_world = nullptr;
	HitboxGen m_colliders;
	Effects m_effects;
	Sprite m_pauseSprite;

	btDbvtBroadphase *_broadphase = new btDbvtBroadphase();
	btCollisionConfiguration *_collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *_dispatcher = new btCollisionDispatcher(_collisionConfiguration);
	btSequentialImpulseConstraintSolver *_solver = new btSequentialImpulseConstraintSolver();
};

