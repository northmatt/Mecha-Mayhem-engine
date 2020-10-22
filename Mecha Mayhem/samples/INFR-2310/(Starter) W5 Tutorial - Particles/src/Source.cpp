/*
Week 5 Tutorial Starter - Created for INFR 2310 at Ontario Tech.
(c) Atiya Nova and Samantha Stahlke 2020
*/

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "CParticleSystem.h"
#include "NOU/GLTFLoader.h"

#include "imgui.h"

#include <memory>
#include <ctime>

using namespace nou;

//Forward declaring our global resources for this demo.
std::unique_ptr<ShaderProgram> prog_texLit, prog_particles;
std::unique_ptr<Mesh> duckMesh;
std::unique_ptr<Texture2D> duckTex, particleTex, particleTex2;
std::unique_ptr<Material> duckMat, particleMat, particleMat2;

//This function will load in our global resources.
//(It's only been separated to make main() a bit cleaner to look at.)
void LoadDefaultResources();

int main()
{
	srand(static_cast<unsigned int>(time(0)));

	App::Init("Week 5 Tutorial - Particles", 800, 800);
	App::SetClearColor(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));

	App::InitImgui();

	LoadDefaultResources();

	//Set up our camera.
	Entity camEntity = Entity::Create();
	auto& cam = camEntity.Add<CCamera>(camEntity);
	cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	camEntity.transform.m_pos = glm::vec3(0.0f, 0.0f, 4.0f);

	//Creating the duck entity.
	Entity duckEntity = Entity::Create();
	duckEntity.Add<CMeshRenderer>(duckEntity, *duckMesh, *duckMat);
	duckEntity.transform.m_scale = glm::vec3(0.005f, 0.005f, 0.005f);
	duckEntity.transform.m_pos = glm::vec3(0.0f, -1.0f, 0.0f);
	duckEntity.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//TODO: Set up our particle system.
	ParticleParam particleData;
	particleData.startColour = glm::vec4(1.f, 0.5f, 0.5f, 1.f);
	particleData.endColour = glm::vec4(0.f, 0.f, 0.f, 0.f);
	particleData.startSize = 0.1f;
	particleData.endSize = 0.1f;
	particleData.maxParticles = 200;
	particleData.emissionRate = 50.f;
	particleData.gravity = 1.f;
	particleData.tanTheta = glm::tan(glm::radians(30.f));



	Entity particleEntity = Entity::Create();
	particleEntity.transform.m_pos = glm::vec3(0.0f, -0.5f, 0.0f);
	particleEntity.Add<CParticleSystem>(particleEntity, *particleMat, particleData);


	// mine
	ParticleParam particleData2;
	particleData2.startColour = glm::vec4(1.f, 1.f, 1.f, 1.f);
	particleData2.endColour = glm::vec4(0.f, 0.f, 0.f, 0.f);
	particleData2.startSize = 0.1f;
	particleData2.endSize = 1.f;
	particleData2.startSpeed = 1.0f;
	particleData2.endSpeed = 0.1f;
	particleData2.maxParticles = 200;
	particleData2.emissionRate = 50.f;
	particleData2.tanTheta = glm::tan(glm::radians(45.f));
	particleData2.startAngle = glm::radians(0.f);
	particleData2.endAngle = glm::radians(360.f);

	Entity particleEntity2 = Entity::Create();
	particleEntity2.transform.m_pos = glm::vec3(0.5f, -0.4f, 0.0f);
	particleEntity2.Add<CParticleSystem>(particleEntity2, *particleMat2, particleData2);

	App::Tick();

	int v1[] = { 0, 360 };
	float v2[] = { 0.1f, 1.f };
	float v3[] = { 1.f, 0.1f };
	int v = 90;
	float v0 = 5.f;
	float vg = 0.f;
	float ve = 50.f;
	float c0[] = { 1.f, 1.f, 1.f, 1.f };
	float c1[] = { 0.f, 0.f, 0.f, 0.f };
	bool blargh = false, blarghe = false;

	//This is our main loop.
	while (!App::IsClosing() && !Input::GetKey(GLFW_KEY_ESCAPE))
	{
		//Start of the frame.
		App::FrameStart();
		float deltaTime = App::GetDeltaTime();
		
		//Updates the camera.
		camEntity.Get<CCamera>().Update();
		 
		particleEntity.Get<CParticleSystem>().Update(deltaTime);
		particleEntity2.Get<CParticleSystem>().Update(deltaTime);

		duckEntity.transform.RecomputeGlobal();
		particleEntity.transform.RecomputeGlobal();
		particleEntity2.transform.RecomputeGlobal();

		//We draw particles with the depth buffer disabled
		//to prevent z-fighting.
		//Custom depth sorting is handled by our particle system.
		glDisable(GL_DEPTH_TEST);
		particleEntity.Get<CParticleSystem>().Draw();
		particleEntity2.Get<CParticleSystem>().Draw();
		glEnable(GL_DEPTH_TEST);
		
		duckEntity.Get<CMeshRenderer>().Draw();

		//For Imgui...
		App::StartImgui();
		auto& parti = particleEntity2.Get<CParticleSystem>().m_data->param;

		ImGui::SetWindowSize(ImVec2(400, 280));

		ImGui::Text("Control the THONK");

		if (ImGui::SliderInt2("Rotate limits", v1, 0.f, 720.f)) {
			parti.startAngle = glm::radians(float(v1[0]));
			parti.endAngle = glm::radians(float(v1[1]));
		}
		if (ImGui::SliderFloat2("Size limits", v2, 0.f, 5.f)) {
			parti.startSize = v2[0];
			parti.endSize = v2[1];
		}
		if (ImGui::SliderFloat2("Speed limits", v3, -5.f, 5.f)) {
			parti.startSpeed = v3[0];
			parti.endSpeed = v3[1];
		}
		if (ImGui::SliderInt("Spread", &v, 0.f, 180.f)) {
			parti.tanTheta = glm::tan(glm::radians(v / 2.f));
		}
		if (ImGui::SliderFloat4("Start RGBA", c0, 0.f, 5.f)) {
			parti.startColour = glm::vec4(c0[0], c0[1], c0[2], c0[3]);
		}
		if (ImGui::SliderFloat4("End RGBA", c1, 0.f, 5.f)) {
			parti.endColour = glm::vec4(c1[0], c1[1], c1[2], c1[3]);
		}
		ImGui::Checkbox("RandColour", &blargh);
		ImGui::SameLine();
		ImGui::Checkbox("SameColour", &blarghe);
		if (blargh) {
			parti.startColour = glm::vec4(rand() % 251 / 100.f, rand() % 251 / 100.f, rand() % 251 / 100.f, 1.f);
			parti.endColour = glm::vec4(rand() % 251 / 100.f, rand() % 251 / 100.f, rand() % 251 / 100.f, 0.f);
		}
		if (blarghe) {
			parti.endColour = parti.startColour;
		}
		if (ImGui::SliderFloat("Life", &v0, 0.f, 15.f)) {
			parti.lifetime = v0;
		}
		if (ImGui::SliderFloat("Gravity", &vg, -5.f, 5.f)) {
			parti.gravity = vg;
		}
		if (ImGui::SliderFloat("Emission", &ve, 0.f, 200.f)) {
			parti.emissionRate = ve;
		}

		//Put any Imgui controls you plan to use for your
		//particle system here (for the exercise).

		App::EndImgui();  

		//This sticks all the drawing we just did on the screen.
		App::SwapBuffers();
	}

	App::Cleanup();

	return 0;
}

void LoadDefaultResources()
{
	//Load in some shaders.
	//Smart pointers will automatically deallocate memory when they go out of scope.
	//Lit and textured shader program.
	auto v_texLit = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	auto f_texLit = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> texLit = { v_texLit.get(), f_texLit.get() };
	prog_texLit = std::make_unique<ShaderProgram>(texLit);

	//Billboarded particles shader program.
	auto v_particles = std::make_unique<Shader>("shaders/particles.vert", GL_VERTEX_SHADER);
	auto g_particles = std::make_unique<Shader>("shaders/particles.geom", GL_GEOMETRY_SHADER);
	auto f_particles = std::make_unique<Shader>("shaders/particles.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> particles = { v_particles.get(), g_particles.get(), f_particles.get() };
	prog_particles = std::make_unique<ShaderProgram>(particles);

	//Load in the ducky model.
	duckMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("duck/Duck.gltf", *duckMesh);

	//Load in textures.
	duckTex = std::make_unique<Texture2D>("duck/DuckCM.png");
	particleTex = std::make_unique<Texture2D>("particle.png");
	particleTex2 = std::make_unique<Texture2D>("thonk.png");
	 
	//Make materials. 
	duckMat = std::make_unique<Material>(*prog_texLit);
	duckMat->AddTexture("albedo", *duckTex);

	particleMat = std::make_unique<Material>(*prog_particles);
	particleMat->AddTexture("albedo", *particleTex);

	particleMat2 = std::make_unique<Material>(*prog_particles);
	particleMat2->AddTexture("albedo", *particleTex2);
}