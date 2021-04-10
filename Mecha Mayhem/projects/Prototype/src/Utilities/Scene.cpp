#include "Scene.h"

std::vector<Scene*> Scene::m_scenes;
Scene* Scene::m_activeScene = nullptr;
size_t Scene::m_nextScene = 0;
bool Scene::m_doSceneChange = false;
bool Scene::m_exitGame = false;

Scene::Scene(const std::string& name, const glm::vec3& gravity, bool physics)
	: m_name(name)
{
	if (physics) {
		_broadphase = new btDbvtBroadphase();
		_collisionConfiguration = new btDefaultCollisionConfiguration();
		_dispatcher = new btCollisionDispatcher(_collisionConfiguration);
		_solver = new btSequentialImpulseConstraintSolver();

		m_world = new btDiscreteDynamicsWorld(
			_dispatcher, _broadphase, _solver, _collisionConfiguration);

		m_world->setGravity(BLM::GLMtoBT(gravity));
	}
}

Scene::~Scene()
{
	//destructors will take care of the rest
	Rendering::hitboxes = nullptr;
	Rendering::effects = nullptr;
	Rendering::frameEffects = nullptr;
	if (m_world != nullptr) {
		delete m_world;
	}
}

entt::registry* Scene::GetRegistry()
{
	return &m_reg;
}

std::string Scene::GetName()
{
	return m_name;
}

void Scene::Init(int windowWidth, int windowHeight)
{
	//generic init, use as guidelines
	ECS::AttachRegistry(&m_reg);
	if (m_world) {
		PhysBody::Init(m_world);
		ECS::AttachWorld(m_world);
		if (!m_colliders.Init(m_world, "maps/map1"))
			std::cout << "map1 failed to load, no collision boxes loaded\n";
	}
	m_frameEffects.Init();

	// for multi cam setup, change the m_camCount variable, and also spawn in reverse order (aka player 1 last)
	auto cameraEnt = ECS::CreateEntity();
	auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
	camCam.SetFovDegrees(60.f).ResizeWindow(windowWidth, windowHeight);
}

Scene* Scene::Reattach()
{
	ECS::AttachRegistry(&m_reg);
	if (m_world) {
		PhysBody::Init(m_world);
		ECS::AttachWorld(m_world);
		Rendering::hitboxes = &m_colliders;
	}
	else {
		PhysBody::Init(nullptr);
		ECS::AttachWorld(nullptr);
		Rendering::hitboxes = nullptr;
	}

	m_frameEffects.Resize(BackEnd::GetWidth(), BackEnd::GetHeight());

	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects.Reattach();

	return this;
}

void Scene::BackEndUpdate()
{
	if (!m_paused) {
		//only update these when paused, so put here
		Player::Update();

		//update components
		m_reg.view<ObjMorphLoader, Transform>().each(
			[](ObjMorphLoader& obj, Transform& trans) {
				obj.Update(Time::dt);
			}
		);
		m_reg.view<Player, PhysBody>().each(
			[](Player& p, PhysBody& body) {
				p.Update(body);
			}
		);
		m_reg.view<Spawner, Transform>().each(
			[&](Spawner& spawn, Transform& trans) {
				spawn.Update(&m_reg, trans.GetGlobalPosition());
			}
		);

		if (m_world != nullptr) {
			m_world->stepSimulation(Time::dt, 10);

			m_reg.view<PhysBody, Transform>().each(
				[](PhysBody& phys, Transform& trans) {
					if (phys.IsDynamic() || phys.Changed())
						trans.SetTransform(phys.GetTransform());
				}
			);
		}

		m_reg.view<Player, Transform>().each(
			[](Player& p, Transform& trans) {
				p.LateUpdate(trans);
			}
		);

		m_effects.Update();
	}

	LateUpdate();

	//always render
	if (m_camCount < 1)
		m_camCount = 1;
	else if (m_camCount > 4)
		m_camCount = 4;

	Sprite::BeginUIDraw(10, m_camCount);

	Rendering::Update(&m_reg, m_camCount, m_paused);

	//dont update shadows if paused
	if (m_frameEffects.GetUsingShadows() && !m_paused)
		Rendering::RenderForShading(&m_reg);

	//once pause buffer works, we can move this or smt
	m_frameEffects.Draw();

	Sprite::PerformUIDraw(m_camCount);

	DrawOverlay();
}

void Scene::QueueSceneChange(size_t index) {
	if (m_nextScene >= m_scenes.size())
		return;

	m_nextScene = index;
	m_doSceneChange = true;
}

void Scene::doSceneChange(GLFWwindow* window) {
	if (m_doSceneChange == false)
		return;

	m_doSceneChange = false;
	m_activeScene->Exit();
	m_activeScene = m_scenes[m_nextScene]->Reattach();
	glfwSetWindowTitle(window, m_activeScene->GetName().c_str());
}

void Scene::UnloadScenes()
{
	m_activeScene = nullptr;
	for (int i(0); i < m_scenes.size(); ++i) {
		if (m_scenes[i] != nullptr) {
			delete m_scenes[i];
			m_scenes[i] = nullptr;
		}
	}
	m_scenes.clear();
}

void Scene::ImGuiFunc()
{
	/*ImGui::SetWindowSize(ImVec2(150, 50));
	ImGui::Text("Empty");*/

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
				else if (name == "Pixel") {
					ImGui::Text("Pixels done by drawing to a smaller buffer, which means no new shader!");
					PixelEffect* effect = (PixelEffect*)(m_frameEffects[i]);
					int pixels = effect->GetPixelCount();
					if (ImGui::SliderInt("PixelCount", &pixels, 4, BackEnd::GetHeight())) {
						effect->SetPixelCount(pixels);
					}
				}
				else if (name == "DepthOfField") {
					ImGui::Text("We got the depth buffer somehow");
					DepthOfFieldEffect* effect = (DepthOfFieldEffect*)(m_frameEffects[i]);
					float depthLimit = effect->GetDepthLimit();
					if (ImGui::SliderFloat("Depth Limit", &depthLimit, 0.f, 1.f)) {
						effect->SetDepthLimit(depthLimit);
					}
					int blur = effect->GetBlurPasses();
					if (ImGui::SliderInt("Blur Passes", &blur, 1, 15)) {
						effect->SetBlurPasses(blur);
					}
				}
				if (name != "N/A")
					if (ImGui::Button("Remove")) {
						m_frameEffects.RemoveEffect(i);
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
			if (ImGui::Button("Bloom")) {
				BloomEffect* effect = new BloomEffect();
				effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
				effect->SetInfo("Bloom");
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
			ImGui::SameLine();
			if (ImGui::Button("Depth Of Field")) {
				DepthOfFieldEffect* effect = new DepthOfFieldEffect();
				effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
				effect->SetInfo("DepthOfField");
				m_frameEffects.AddEffect(effect);
				effect->SetDrawBuffer(m_frameEffects.GetDrawBuffer());
				effect = nullptr;
			}
		}
		else {
			ImGui::Text("Max effects added");
		}
	}

}