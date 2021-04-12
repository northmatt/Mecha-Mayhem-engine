#include "Rendering.h"
#include "Time.h"

namespace Rendering {

	void Init(int width, int height)
	{
		//put any pause effects here, maybe
		orthoVP.SetOrthoHeight(10.f).SetNear(-100.f).Setfar(100.f)
			.SetIsOrtho(true).ResizeWindow(width, height).SetPosition(BLM::GLMzero);
	}
	
	void Update(entt::registry* reg, int numOfCams, bool paused)
	{
		//sending clear colour first because of illum buffer
		glClearColor(BackColour.x, BackColour.y, BackColour.z, 0.3f);
		frameEffects->Clear(paused);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//frameEffects->Bind();

		auto objView = reg->view<ObjLoader, Transform>();
		auto textObjView = reg->view<MultiTextObj, Transform>();
		auto morphView = reg->view<ObjMorphLoader, Transform>();
		auto spriteView = reg->view<Sprite, Transform>();
		auto playerView = reg->view<Player, Transform>();
		auto cameraView = reg->view<Camera, Transform>();
		auto spawnerView = reg->view<Spawner, Transform>();

		//reserve some queue size
		ObjLoader::BeginDraw(objView.size());
		ObjMorphLoader::BeginDraw(morphView.size() + spawnerView.size(), effects->size());

		//send all objs to the vectors
		objView.each(
			[](ObjLoader& obj, Transform& trans) {
				obj.Draw(trans.GetModel());
			}
		);

		//send all the morph objs to the vectors
		morphView.each(
			[](ObjMorphLoader& obj, Transform& trans) {
				obj.Draw(trans.GetModel());
			}
		);

		spawnerView.each(
			[](Spawner& spawn, Transform& trans) {
				spawn.Render(trans.GetModel());
			}
		);

		//draw scene specific stuff (only needs to be sent once)
		if (hitboxes != nullptr) hitboxes->Render();
		if (effects != nullptr) effects->Render();


		int height = BackEnd::GetHalfHeight();
		int width = BackEnd::GetHalfWidth();

		short count = 0;
		for (auto cam : cameraView)
		{
			FrameEffects::Bind();

			//glViewport((count % 2) * width, ((count < 2) && (numOfCams > 2)) * height,
			//    width * ((numOfCams == 1) + 1), height * (2 - (numOfCams > 2)));

			if (numOfCams > 2)
				glViewport(((count % 2) * width), ((count < 2) * height), width, height);
			else if (numOfCams == 2)
				glViewport((count * width), 0, width, height * 2);
			else
				glViewport(0, 0, width * 2, height * 2);

			Transform& camTrans = cameraView.get<Transform>(cam);
			glm::mat4 view = glm::inverse(camTrans.GetModel());

			Camera& camCam = cameraView.get<Camera>(cam);

			camCam.SetPosition(camTrans.GetGlobalPosition()).
				SetForward(camTrans.GetForwards());

			frameEffects->SetCamPos(camTrans.GetGlobalPosition(), count);

			ObjLoader::BeginTempDraw();
			ObjMorphLoader::BeginTempDraw();
			Sprite::BeginDraw(spriteView.size());
			
			//sprites use VP in the draw function
			glm::mat4 VP = camCam.GetProjection() * view;
			frameEffects->SetCamVP(VP);
			spriteView.each(
				[&](Sprite& spr, Transform& trans) {
					spr.Draw(VP, trans.GetModel());
				}
			);

			//draw all players (this has to be here because of the possibility of removing the player, and UI)
			playerView.each(
				[&](Player& p, Transform& trans) {
					p.Draw(trans.GetModel(), count, numOfCams, paused);
				}
			);

			//map drawn first for transparency
			textObjView.each(
				[&](MultiTextObj& obj, Transform& trans) {
					obj.Draw(trans.GetModel(), view, camCam, glm::vec3(0));
				}
			);

			//do all the draws
			ObjLoader::PerformDraw(view, camCam, DefaultColour);
			ObjMorphLoader::PerformDraw(view, camCam, DefaultColour);
			Sprite::PerformDraw();

			FrameEffects::BindTransparency();

			ObjMorphLoader::PerformDrawTrans(view, camCam);

			FrameEffects::UnBindTransparency();
			
			//frameEffects->Bind();

			//exit even if some cams haven't been checked, because only the amount specified should render
			if (++count >= numOfCams)
				break;
		}

		//frameEffects->UnBind();

		FrameEffects::SetCamCount(numOfCams);
	}

	void RenderForShading(entt::registry* reg)
	{
		glViewport(0, 0, FrameEffects::shadowWidth, FrameEffects::shadowHeight);

		glm::mat4 lightVPMatrix = frameEffects->GetShadowVP();

		auto objView = reg->view<ObjLoader, Transform>();
		auto textObjView = reg->view<MultiTextObj, Transform>();
		auto morphView = reg->view<ObjMorphLoader, Transform>();
		auto spriteView = reg->view<Sprite, Transform>();
		auto playerView = reg->view<Player, Transform>();
		auto spawnerView = reg->view<Spawner, Transform>();

		//for (int i(0); i < 2; ++i) {
			//frameEffects->BindShadowMap(i);

			FrameEffects::BindShadowMap();

			//reserve some queue size
			ObjLoader::BeginDraw(objView.size());
			ObjMorphLoader::BeginDraw(morphView.size());
			Sprite::BeginDraw(spriteView.size());
			ObjLoader::BeginTempDraw();
			ObjMorphLoader::BeginTempDraw();

			objView.each([](ObjLoader& obj, Transform& trans) {
				if (obj.GetCastShadows())
					obj.Draw(trans.GetModel());
				});

			morphView.each([](ObjMorphLoader& obj, Transform& trans) {
				if (obj.GetCastShadows())
					obj.Draw(trans.GetModel());
				});

			spriteView.each([](Sprite& spr, Transform& trans) {
				if (spr.GetCastShadows())
					spr.Draw(BLM::GLMMat, trans.GetModel());
				});

			spawnerView.each([](Spawner& spawn, Transform& trans) {
				spawn.Render(trans.GetModel(), false);
				});

			//draw all players, cams are limited from 0-3, so this ignores all cams
			playerView.each([](Player& p, Transform& trans) {
				p.Draw(trans.GetModel(), 4, 0, false);
				});

			//draw scene specific stuff (might want to remove this, if you don't want lasers to cast shadows)
			if (effects != nullptr) effects->Render();

			//do all the draws
			ObjLoader::PerformDrawShadow(lightVPMatrix);
			ObjMorphLoader::PerformDrawShadow(lightVPMatrix);

			//make sure this runs after ObjDraw
			Sprite::PerformDrawShadow(lightVPMatrix);

			//map drawn last becuase shader reuse lol
			textObjView.each([&](MultiTextObj& obj, Transform& trans) {
				obj.DrawShadow(lightVPMatrix * trans.GetModel());
				});
		//}
		FrameEffects::UnBindShadowMap();
	}

	/*void DrawPauseScreen(Sprite image)
	{
		image.DrawSingle(orthoVP.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, -100, 1
		));
	}*/

	std::array<glm::vec3, MAX_LIGHTS> LightsColour = {
	   glm::vec3(20.f),
	   glm::vec3(1.5f, 1.5f, 0.f),
	   glm::vec3(.25f), glm::vec3(.25f), glm::vec3(.25f), glm::vec3(.25f),
	   glm::vec3(.25f), glm::vec3(.25f), glm::vec3(.25f), glm::vec3(.25f)
	};
	std::array<glm::vec3, MAX_LIGHTS> LightsPos = {
		glm::vec3(0.f), glm::vec3(0.f),	glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f),
		glm::vec3(0.f),	glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f)
	};
	size_t LightCount = 0;
	glm::vec3 BackColour = { 0.2f, 0.2f, 0.2f };
	glm::vec3 DefaultColour = glm::vec3(1.f);

	HitboxGen* hitboxes = nullptr;
	Effects* effects = nullptr;
	FrameEffects* frameEffects = nullptr;
	Camera orthoVP;
}