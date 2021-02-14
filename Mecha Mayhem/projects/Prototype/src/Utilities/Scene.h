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
			Rendering::doAmbient = !Rendering::doAmbient;
		}
		
		if (ImGui::Button("Diffuse Toggle", ImVec2(140.0f, 20.0f))) {
			Rendering::doDiffuse = !Rendering::doDiffuse;
		}
		
		if (ImGui::Button("Spec Toggle", ImVec2(140.0f, 20.0f))) {
			Rendering::doSpecular = !Rendering::doSpecular;
		}

		if (ImGui::Button("Tex Toggle", ImVec2(140.0f, 20.0f))) {
			Rendering::doTex = !Rendering::doTex;
		}

		if (ImGui::Button("LUT Toggle", ImVec2(140.0f, 20.0f))) {
			doCol = !doCol;
		}

		if (ImGui::Button("Bloom Toggle", ImVec2(140.0f, 20.0f))) {
			doBloom = !doBloom;
		}

		float *downscale = new float();
		float *threshold = new float();
		int *passes = new int();
		bool changeBloom = false;

		auto Blooms = _activeScene->GetRegistry()->view<BloomEffect>();
		Blooms.each([=](BloomEffect& buf) {
			*downscale = buf.GetDownscale();
			*threshold = buf.GetThreshold();
			*passes = buf.GetPasses();
		});

		if (ImGui::InputFloat("Bloom Downscale", downscale)) {
			if (*downscale < 0)
				*downscale = 0;
			changeBloom = true;
		}

		if (ImGui::InputFloat("Bloom Threshold", threshold)) {
			if (*threshold < 0)
				*threshold = 0;
			changeBloom = true;
		}

		if (ImGui::InputInt("Bloom Passes", passes)) {
			if (*passes < 0)
				*passes = 0;
			changeBloom = true;
		}

		if (changeBloom) {
			Blooms.each([=](BloomEffect& buf) {
				buf.SetDownscale(*downscale);
				buf.SetThreshold(*threshold);
				buf.SetPasses(*passes);
			});
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

	bool doCol = false;
	bool doBloom = true;

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

