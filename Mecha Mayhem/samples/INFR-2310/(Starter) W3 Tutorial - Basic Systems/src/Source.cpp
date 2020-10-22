/*
Week 3 Tutorial Starter - Created for INFR 2310 at Ontario Tech.
(c) Atiya Nova and Samantha Stahlke 2020
*/

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "Sprites/CSpriteRenderer.h"
#include "Sprites/CSpriteAnimator.h"
#include "CKnightFSM.h"

#include "imgui.h"

#include <memory>

using namespace nou;

int main() 
{
	App::Init("Week 3 Tutorial - Sprites", 800, 800);
	App::SetClearColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	App::InitImgui();

	//Load in some shaders.
	auto v_sprite = std::make_unique<Shader>("shaders/texturedunlit.vert", GL_VERTEX_SHADER);
	auto f_sprite = std::make_unique<Shader>("shaders/texturedunlit.frag", GL_FRAGMENT_SHADER);

	auto prog_sprite = ShaderProgram({ v_sprite.get(), f_sprite.get() });

	//Load in sprites.
	Texture2D boomTex = Texture2D("explosion.png", true);
	Texture2D knightTex = Texture2D("knight.png", true);
	Texture2D fairyTex = Texture2D("fairy.png", true);

	Material boomMat(prog_sprite);
	boomMat.AddTexture("albedo", boomTex);

	Material knightMat(prog_sprite);
	knightMat.AddTexture("albedo", knightTex);

	Material fairyMat(prog_sprite);
	fairyMat.AddTexture("albedo", fairyTex);
	
	//TODO: Load in explosion spritesheet.
	auto boomSheet = std::make_unique<Spritesheet>(boomTex, glm::vec2(222.f, 222.f));
	boomSheet->AddAnimation("boom", 0, 27, 30.f);
	boomSheet->SetDefaultFrame(27);

	//Load in knight spritesheet, add animations.
	auto knightSheet = std::make_unique<Spritesheet>(knightTex, glm::vec2(64.0f, 64.0f));
	knightSheet->AddAnimation(CKnightFSM::idleClip, 0, 4, 12.0f);
	knightSheet->AddAnimation(CKnightFSM::runClip, 5, 12, 12.0f);
	knightSheet->AddAnimation(CKnightFSM::attackClip, 19, 21, 12.0f);

	//Load in fairy spritesheet, add animations.
	auto fairySheet = std::make_unique<Spritesheet>(fairyTex, glm::vec2(29.f, 28.f));
	fairySheet->AddAnimation(CFairyFSM::idleClip, 0, 3, 12.0f);
	fairySheet->AddAnimation(CFairyFSM::scaredClip, 4, 7, 12.0f);
	fairySheet->AddAnimation(CFairyFSM::attackClip, 8, 11, 12.0f);

	//Set up our camera.
	Entity camEntity = Entity::Create();
	auto& cam = camEntity.Add<CCamera>(camEntity);
	cam.Ortho(-400.0f, 400.0f, -400.0f, 400.0f, 0.1f, 100.0f);
	camEntity.transform.m_pos = glm::vec3(0.0f, 0.0f, 2.0f);

	//TODO: Create explosion entity.
	Entity boomEntity = Entity::Create();
	boomEntity.Add<CSpriteRenderer>(boomEntity, *boomSheet, boomMat);
	auto& boomAnim = boomEntity.Add<CSpriteAnimator>(boomEntity, *boomSheet);

	//Create the knight entity.
	Entity knightEntity = Entity::Create();
	knightEntity.transform.m_scale = glm::vec3(2.0f, 2.0f, 2.0f);
	knightEntity.Add<CSpriteRenderer>(knightEntity, *knightSheet, knightMat);
	auto& knightAnim = knightEntity.Add<CSpriteAnimator>(knightEntity, *knightSheet);
	knightEntity.Add<CKnightFSM>(knightEntity);

	//Create the fairy entity.
	Entity fairyEntity = Entity::Create();
	fairyEntity.transform.m_scale = glm::vec3(7.5f, 7.5f, 7.5f);
	fairyEntity.Add<CSpriteRenderer>(fairyEntity, *fairySheet, fairyMat);
	auto& fairyAnim = fairyEntity.Add<CSpriteAnimator>(fairyEntity, *fairySheet);
	fairyEntity.Add<CFairyFSM>(fairyEntity);

	App::Tick();

	//Disabling the depth buffer.
	//(The reason we do this is so we can naively draw sprites on top
	//of each other in the order we call Draw().)
	glDisable(GL_DEPTH_TEST);

	//Disable backface culling.
	//(The reason we disable culling is to make it simpler for us to 
	//flip sprites without making OpenGL think we're drawing polygons
	//that shouldn't be visible.)
	glDisable(GL_CULL_FACE);
	
	//This is our main loop.
	while (!App::IsClosing() && !Input::GetKey(GLFW_KEY_ESCAPE))
	{
		//Start of the frame.
		App::FrameStart();
		float deltaTime = App::GetDeltaTime();

		//TODO: Control our knight.
		bool moving = Input::GetKey(GLFW_KEY_RIGHT) || Input::GetKey(GLFW_KEY_LEFT);
		knightEntity.Get<CKnightFSM>().SetVariable("moving", moving);
		
		if (moving) {
			bool flip = Input::GetKey(GLFW_KEY_LEFT);

			knightEntity.transform.m_scale.x = (flip ? -2.f : 2.f);
			knightEntity.transform.m_pos.x += (flip ? -100.f : 100.f) * deltaTime;
		}

		moving = Input::GetKey(GLFW_KEY_W) || Input::GetKey(GLFW_KEY_A) ||
			Input::GetKey(GLFW_KEY_S) || Input::GetKey(GLFW_KEY_D);

		if (moving) {
			if (Input::GetKey(GLFW_KEY_A) || Input::GetKey(GLFW_KEY_D)) {
				bool flip = Input::GetKey(GLFW_KEY_A);

				fairyEntity.transform.m_scale.x = (flip ? 7.5f : -7.5f);
				fairyEntity.transform.m_pos.x += (flip ? -150.f : 150.f) * deltaTime;
			}
			if (Input::GetKey(GLFW_KEY_W) || Input::GetKey(GLFW_KEY_S)) {
				fairyEntity.transform.m_pos.y += (Input::GetKey(GLFW_KEY_S) ? -150.f : 150.f) * deltaTime;
			}
		}
		fairyEntity.Get<CFairyFSM>().SetVariable("moving", moving);

		fairyEntity.Get<CFairyFSM>().SetVariable("attack", Input::GetKey(GLFW_KEY_SPACE));

		//Updates all the entities.
		camEntity.Get<CCamera>().Update();
		//TODO: Update explosion entity.
		knightEntity.Get<CKnightFSM>().Update();
		fairyEntity.Get<CFairyFSM>().Update();
		knightEntity.Get<CSpriteAnimator>().Update(deltaTime);
		boomEntity.Get<CSpriteAnimator>().Update(deltaTime);
		fairyEntity.Get<CSpriteAnimator>().Update(deltaTime);
		
		//Recomputes global matrices.
		//TODO: Update explosion entity.
		knightEntity.transform.RecomputeGlobal();
		boomEntity.transform.RecomputeGlobal();
		fairyEntity.transform.RecomputeGlobal();

		//Draws the sprites.
		//TODO: Draw explosion entity.
		knightEntity.Get<CSpriteRenderer>().Draw();	
		boomEntity.Get<CSpriteRenderer>().Draw();
		fairyEntity.Get<CSpriteRenderer>().Draw();

		//For Imgui stuff...
		App::StartImgui();

		//TODO: Create Imgui panel.
		//TODO: Create Imgui button and play explosion on press.
		static bool panelOpen = true;
		ImGui::Begin("Coffee is great", &panelOpen, ImVec2(300, 200));

		if (ImGui::Button("Boom!"))
			boomEntity.Get<CSpriteAnimator>().PlayOnce("boom");

		if (ImGui::Button("Attack!"))
			knightEntity.Get<CKnightFSM>().SetTrigger("attack");

		ImGui::Text("Fairy has 3 states:");
		ImGui::Text("idle - idle");
		ImGui::Text("scared - when moving");
		ImGui::Text("attack - hold \"space\" to engage gun");
		ImGui::Text("wasd to move");

		ImGui::End();
		App::EndImgui();

		//This sticks all the drawing we just did on the screen.
		App::SwapBuffers();
	}

	App::Cleanup();

	return 0; 
} 
