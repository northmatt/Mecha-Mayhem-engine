/*
Week 2 Tutorial Starter - Created for INFR 2310 at Ontario Tech.
(c) Atiya Nova and Samantha Stahlke 2020
*/

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/GLTFLoader.h"

#include "Logging.h"

#include <memory>

using namespace nou;

//TODO: Write templated LERP function.
template<typename T>
T LERP(const T& p0, const T& p1, float t) {
	return (1.f - t) * p0 + t * p1;
}

int main()
{
	App::Init("Week 1 Tutorial - LERP", 800, 800);
	App::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	//Load in some shaders.
	//Smart pointers will automatically deallocate memory when they go out of scope.
	auto v_texLit = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	auto f_texLit = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);

	auto prog_texLit = ShaderProgram({ v_texLit.get(), f_texLit.get() });

	//TODO: Load in duck model and texture.
	Mesh duckMesh;
	GLTF::LoadMesh("duck/Duck.gltf", duckMesh);
	Texture2D duckTex = Texture2D("duck/DuckCM.png");
	Material duckMat(prog_texLit);
	duckMat.AddTexture("albedo", duckTex);

	Mesh bladeMesh;
	GLTF::LoadMesh("blade.gltf", bladeMesh);
	Material bladeMat(prog_texLit);
	Texture2D bladeTex = Texture2D("bun.jpg");
	bladeMat.AddTexture("albedo", bladeTex);

	//TODO: Set up camera entity and transform.
	Entity camEntity = Entity::Create();
	auto& cam = camEntity.Add<CCamera>(camEntity);
	cam.Perspective(60.f, 1.f, 0.1f, 100.f);
	camEntity.transform.m_pos = glm::vec3(0.f, 0.f, 4.f);

	//TODO: Create our duck entity.
	Entity duckEntity = Entity::Create();
	duckEntity.Add<CMeshRenderer>(duckEntity, duckMesh, duckMat);
	duckEntity.transform.m_scale = glm::vec3(0.005f, 0.005f, 0.005f);
	duckEntity.transform.m_pos = glm::vec3(-1.f, -1.f, 0.f);
	duckEntity.transform.m_rotation = glm::angleAxis(glm::radians(-30.f), glm::vec3(0.f, 1.f, 0.f));

	Entity bladeEntity = Entity::Create();
	bladeEntity.Add<CMeshRenderer>(bladeEntity, bladeMesh, bladeMat);
	bladeEntity.transform.m_scale = glm::vec3(0.3f, 0.3f, 0.3f);
	bladeEntity.transform.m_pos = glm::vec3(1.f, -1.f, 0.f);

	//variables for LERPing position
	glm::vec3 origPos = glm::vec3(-1.f, -1.f, 0.f), newPos = glm::vec3(-1.f, 1.f, 0.f);
	float posTimer = 0.f, posTimeLimit = 2.f;
	bool forwards = true;
	glm::vec3 origColour = glm::vec3(1.f, 1.f, 1.f), newColour = glm::vec3(1.f, 0.f, 0.5f);
	float colourTimer = 0.f, colourTimeLimit = 0.5f;
	bool forwards2 = true;
	float origAngle = 0.f, newAngle = 360.f;
	float angleTimer = 0.f, angleTimeLimit = 10.f;
	glm::vec3 origSize = { 0.0005f, 0.0005f, 0.0005f }, newSize = { 0.01f, 0.01f, 0.01f };
	float sizeTimer = 0.f, sizeTimeLimit = 5.f;
	bool forwards3 = true;

	glm::vec3 origPos2 = glm::vec3(1.f, -1.f, 0.f), newPos2 = glm::vec3(1.f, 1.f, 0.f);
	float posTimer2 = 0.f, posTimeLimit2 = 3.f;
	bool forwards1 = true;
	glm::vec3 origColour2 = glm::vec3(0.f, 0.f, 0.f), newColour2 = glm::vec3(1.f, 1.f, 1.f);
	float colourTimer2 = 0.f, colourTimeLimit2 = 0.5f;
	bool forwards21 = true;
	float origAngle2 = 0.f, newAngle2 = 360.f;
	float angleTimer2 = 0.f, angleTimeLimit2 = 7.5f;

	App::Tick();

	//This is our main loop.
	while (!App::IsClosing() && !Input::GetKey(GLFW_KEY_ESCAPE))
	{
		//Start of the frame.
		App::FrameStart();
		float deltaTime = App::GetDeltaTime();

		//TODO: LERP stuff!
		if (forwards) {
			posTimer += deltaTime;
			if (posTimer >= posTimeLimit)
				forwards = false;
		}
		else {
			posTimer -= deltaTime;
			if (posTimer <= 0.f)
				forwards = true;
		}
		if (forwards1) {
			posTimer2 += deltaTime;
			if (posTimer2 >= posTimeLimit2)
				forwards1 = false;
		}
		else {
			posTimer2 -= deltaTime;
			if (posTimer2 <= 0.f)
				forwards1 = true;
		}
		if (forwards21) {
			colourTimer2 += deltaTime;
			if (colourTimer2 >= colourTimeLimit2) {
				forwards21 = false;
				origColour2 = glm::vec3((rand() % 11) / 10.f, (rand() % 11) / 10.f, (rand() % 11) / 10.f);
			}
		}
		else {
			colourTimer2 -= deltaTime;
			if (colourTimer2 <= 0.f) {
				forwards21 = true;
				newColour2 = glm::vec3((rand() % 11) / 10.f, (rand() % 11) / 10.f, (rand() % 11) / 10.f);
			}
		}
		if (forwards2) {
			colourTimer += deltaTime;
			if (colourTimer >= colourTimeLimit)
				forwards2 = false;
		}
		else {
			colourTimer -= deltaTime;
			if (colourTimer <= 0.f)
				forwards2 = true;
		}
		if (forwards3) {
			sizeTimer += deltaTime;
			if (sizeTimer >= sizeTimeLimit)
				forwards3 = false;
		}
		else {
			sizeTimer -= deltaTime;
			if (sizeTimer <= 0.f)
				forwards3 = true;
		}
		angleTimer += deltaTime;
		if (angleTimer >= angleTimeLimit)
			angleTimer = 0;
		angleTimer2 += deltaTime;
		if (angleTimer2 >= angleTimeLimit2)
			angleTimer2 = 0;


		//calculate
		float tPos = posTimer / posTimeLimit;
		duckEntity.transform.m_pos = LERP(origPos, newPos, tPos);
		tPos = colourTimer / colourTimeLimit;
		duckMat.m_color = LERP(origColour, newColour, tPos);
		tPos = angleTimer / angleTimeLimit;
		duckEntity.transform.m_rotation =
			glm::angleAxis(glm::radians(LERP(origAngle, newAngle, tPos)), glm::vec3(0.f, 1.f, 0.f));
		tPos = sizeTimer / sizeTimeLimit;
		duckEntity.transform.m_scale = LERP(origSize, newSize, tPos);

		tPos = posTimer2 / posTimeLimit2;
		bladeEntity.transform.m_pos = LERP(origPos2, newPos2, tPos);
		tPos = angleTimer2 / angleTimeLimit2;
		bladeEntity.transform.m_rotation =
			glm::angleAxis(glm::radians(LERP(origAngle2, newAngle2, tPos)), glm::vec3(1.f, 1.f, 1.f));
		tPos = colourTimer2 / colourTimeLimit2;
		bladeMat.m_color = LERP(origColour2, newColour2, tPos);


		//TODO: Update camera, update duck transform, draw duck...
		camEntity.Get<CCamera>().Update();

		duckEntity.transform.RecomputeGlobal();
		duckEntity.Get<CMeshRenderer>().Draw();
		bladeEntity.transform.RecomputeGlobal();
		bladeEntity.Get<CMeshRenderer>().Draw();

		//This sticks all the drawing we just did on the screen.
		App::SwapBuffers();
	}

	App::Cleanup();

	return 0; 
} 
