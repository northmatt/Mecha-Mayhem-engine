#pragma once
#include "Utilities/Scene.h"

class DemoScene : public Scene
{
public:
	DemoScene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual Scene* Reattach() override;
	virtual void ImGuiFunc() override {
		if (ImGui::CollapsingHeader("Post Processing Effects"))
		{
			ImGui::Text(("Number of effects: " + std::to_string(m_frameEffects.size())).c_str());
			ImGui::SliderInt("max amt of effects", &maxEffectCount, 0, 5);
			for (int i(0); i < m_frameEffects.size(); ++i) {
				std::string name = m_frameEffects[i]->Info();
				if (ImGui::TreeNode((std::to_string(i + 1) + ": " + name).c_str())) {
					if (name == "Bloom") {
						BloomEffect* effect = (BloomEffect*)(m_frameEffects[i]);
						float threshold = effect->GetTreshold();
						if (ImGui::SliderFloat("Treshold", &threshold, 0.f, 1.f)) {
							effect->SetThreshold(threshold);
						}
						float radius = effect->GetRadius();
						if (ImGui::SliderFloat("Radius", &radius, 0.f, 15.f)) {
							effect->SetRadius(radius);
						}
						int blur = effect->GetBlurCount();
						if (ImGui::SliderInt("Blur Passes", &blur, 1, 15)) {
							effect->SetBlurCount(blur);
						}
					}
					else if (name == "Greyscale") {
						GreyscaleEffect* effect = (GreyscaleEffect*)(m_frameEffects[i]);
						float intensity = effect->GetIntensity();
						if (ImGui::SliderFloat("Intensity", &intensity, 0.f, 1.f)) {
							effect->SetIntensity(intensity);
						}
					}
					else if (name == "Sepia") {
						SepiaEffect* effect = (SepiaEffect*)(m_frameEffects[i]);
						float intensity = effect->GetIntensity();
						if (ImGui::SliderFloat("Intensity", &intensity, 0.f, 1.f)) {
							effect->SetIntensity(intensity);
						}
					}
					else if (name == "Toon") {
						ImGui::Text("Toon bands based on average of rgb");
						ToonEffect* effect = (ToonEffect*)(m_frameEffects[i]);
						int bands = effect->GetBands();
						if (ImGui::SliderInt("Bands", &bands, 3, 15)) {
							effect->SetBands(bands);
						}
					}
					else if (name == "Pixel") {
						ImGui::Text("Pixels done by drawing to a smaller buffer, which means no new shader!");
						PixelEffect* effect = (PixelEffect*)(m_frameEffects[i]);
						int pixels = effect->GetPixelCount();
						if (ImGui::SliderInt("PixelCount", &pixels, 4, BackEnd::GetHalfHeight() * 2)) {
							effect->SetPixelCount(pixels);
						}
					}
					if (ImGui::Button("Remove")) {
						m_frameEffects.RemoveEffect(i);
						//ImGui::SetNextItemOpen(false);
					}
					ImGui::TreePop();
				}
			}
			if (m_frameEffects.size() < maxEffectCount) {
				if (ImGui::Button("Greyscale")) {
					GreyscaleEffect* effect = new GreyscaleEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Greyscale");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("Sepia")) {
					SepiaEffect* effect = new SepiaEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Sepia");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("Bloom")) {
					BloomEffect* effect = new BloomEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Bloom");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("\"Toon Shading\"")) {
					ToonEffect* effect = new ToonEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Toon");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("Pixelataion")) {
					PixelEffect* effect = new PixelEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Pixel");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
			}
			else {
				ImGui::Text("Max effects added");
			}
		}
	}

private:

	size_t killGoal = 10;

	float camDistance = 2.5f;
	float m_timer = 0.f;

	entt::entity bodyEnt[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity Head[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity cameraEnt[4] = { entt::null, entt::null, entt::null, entt::null };
};

