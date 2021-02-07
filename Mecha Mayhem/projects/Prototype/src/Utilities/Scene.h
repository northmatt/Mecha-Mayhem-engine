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
		/*ImGui::SetWindowSize(ImVec2(150, 50));
		ImGui::Text("Empty");*/
		if (ImGui::CollapsingHeader("Options")) {
			if (ImGui::Button("1: no lighting")) {
				ObjLoader::usingAmbient = false;
				ObjLoader::usingDiffuse = false;
				ObjLoader::usingSpecular = false;
				ObjLoader::noLighting = true;
				ObjLoader::toonShading = false;
			}
			if (ImGui::Button("2: ambient only")) {
				ObjLoader::usingAmbient = true;
				ObjLoader::usingDiffuse = false;
				ObjLoader::usingSpecular = false;
				ObjLoader::noLighting = false;
				ObjLoader::toonShading = false;
			}
			if (ImGui::Button("3: specular only")) {
				ObjLoader::usingAmbient = false;
				ObjLoader::usingDiffuse = false;
				ObjLoader::usingSpecular = true;
				ObjLoader::noLighting = false;
				ObjLoader::toonShading = false;
			}
			if (ImGui::Button("4: ambient + specular")) {
				ObjLoader::usingAmbient = true;
				ObjLoader::usingDiffuse = true;
				ObjLoader::usingSpecular = true;
				ObjLoader::noLighting = false;
				ObjLoader::toonShading = false;
			}
			if (ImGui::Button("5: ambient + specular + toon (aka all)")) {
				ObjLoader::usingAmbient = true;
				ObjLoader::usingDiffuse = true;
				ObjLoader::usingSpecular = true;
				ObjLoader::noLighting = false;
				ObjLoader::toonShading = true;
			}
			ImGui::Checkbox("6: diffuse ramp", &ObjLoader::usingDiffuseRamp);
			ImGui::Checkbox("7: specular ramp", &ObjLoader::usingSpecularRamp);
			ImGui::Text("For colour grading, visit the Post processing Effects header");
		}
		if (ImGui::CollapsingHeader("Individual Toggles")) {
			ImGui::Checkbox("ambient", &ObjLoader::usingAmbient);
			ImGui::Checkbox("diffuse", &ObjLoader::usingDiffuse);
			ImGui::Checkbox("specular", &ObjLoader::usingSpecular);
			ImGui::Checkbox("no lighting", &ObjLoader::noLighting);
			ImGui::Checkbox("toon shading", &ObjLoader::toonShading);
			ImGui::Checkbox("diffuse ramp", &ObjLoader::usingDiffuseRamp);
			ImGui::Checkbox("specular ramp", &ObjLoader::usingSpecularRamp);
		}
		if (ImGui::CollapsingHeader("Post Processing Effects")) {
			ImGui::Text(("Number of effects: " + std::to_string(m_frameEffects.size())).c_str());
			ImGui::SliderInt("max amt of effects", &maxEffectCount, 0, 5);
			for (int i(0); i < m_frameEffects.size(); ++i) {
				if (ImGui::Button((std::to_string(i + 1) + ": Remove " + m_frameEffects[i]->Info()).c_str())) {
					m_frameEffects.RemoveEffect(i);
					break;
				}
			}
			if (m_frameEffects.size() < maxEffectCount) {
				if (ImGui::Button("warm")) {
					ColourCorrection* effect = new ColourCorrection();
					effect->Init(BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);
					effect->SetCube(ObjLoader::warm);
					effect->SetInfo("Warm");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("cool")) {
					ColourCorrection* effect = new ColourCorrection();
					effect->Init(BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);
					effect->SetCube(ObjLoader::cold);
					effect->SetInfo("Cold");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("custom")) {
					ColourCorrection* effect = new ColourCorrection();
					effect->Init(BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);
					effect->SetCube(ObjLoader::custom);
					effect->SetInfo("Custom");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
			}
			else { ImGui::Text("Max effects added"); }
		}
	};

	virtual void BackEndUpdate() final;
	virtual int ChangeScene(int sceneCount) final;
	virtual bool ExitTest() final {
		return m_exitGame;
	}

protected:
	std::string m_name;
	short m_camCount = 1;
	int m_nextScene = -1;
	int maxEffectCount = 1;
	bool m_paused = false;
	bool m_exitGame = false;

	entt::registry m_reg;
	btDiscreteDynamicsWorld *m_world = nullptr;
	HitboxGen m_colliders;
	Effects m_effects;
	Sprite m_pauseSprite;
	FrameEffects m_frameEffects;

	btDbvtBroadphase *_broadphase = new btDbvtBroadphase();
	btCollisionConfiguration *_collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *_dispatcher = new btCollisionDispatcher(_collisionConfiguration);
	btSequentialImpulseConstraintSolver *_solver = new btSequentialImpulseConstraintSolver();
};

