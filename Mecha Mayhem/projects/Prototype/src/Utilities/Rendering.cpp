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
		frameEffects->Clear();

		glClearColor(BackColour.x, BackColour.y, BackColour.z, BackColour.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frameEffects->Bind();

		auto objView = reg->view<ObjLoader, Transform>();
		auto textObjView = reg->view<MultiTextObj, Transform>();
		auto morphView = reg->view<ObjMorphLoader, Transform>();
		auto spriteView = reg->view<Sprite, Transform>();
		auto playerView = reg->view<Player, PhysBody, Transform>();
		auto cameraView = reg->view<Camera, Transform>();
		auto spawnerView = reg->view<Spawner, Transform>();

		int height = BackEnd::GetHalfHeight();
		int width = BackEnd::GetHalfWidth();

		short count = 0;
		for (auto cam : cameraView)
		{
			//glViewport((count % 2 == 0 ? width : 0), ((count < 3) && (numOfCams > 2) ? height : 0),
			//    width * (numOfCams == 1 ? 2 : 1), height * (numOfCams > 2 ? 1 : 2));

			if (numOfCams > 2)
				glViewport(((count % 2) * width), (count < 2 ? height : 0), width, height);
			else if (numOfCams == 2)
				glViewport((count * width), 0, width, height * 2);
			else
				glViewport(0, 0, width * 2, height * 2);

			Transform& camTrans = cameraView.get<Transform>(cam);
			glm::mat4 view = glm::inverse(camTrans.GetModel());

			Camera& camCam = cameraView.get<Camera>(cam);

			camCam.SetPosition(camTrans.GetGlobalPosition()).
				SetForward(camTrans.GetForwards());

			//reserve some queue size
			ObjLoader::BeginDraw(objView.size());
			ObjMorphLoader::BeginDraw(morphView.size() + spawnerView.size() + playerView.size());
			//number of ui elements
			Sprite::BeginDraw(spriteView.size() + 6);

			//draw all the objs
			objView.each(
				[](ObjLoader& obj, Transform& trans) {
					obj.Draw(trans.GetModel());
				}
			);

			//draw all the morph objs
			morphView.each(
				[&](ObjMorphLoader& obj, Transform& trans) {
					if ((count == 0) && !paused) obj.Update(Time::dt);
					obj.Draw(trans.GetModel());
				}
			);

			glm::mat4 VP = camCam.GetProjection() * view;
			spriteView.each(
				[&](Sprite& spr, Transform& trans) {
					spr.Draw(VP, trans.GetModel());
				}
			);

			spawnerView.each(
				[&](Spawner& spawn, Transform& trans) {
					if ((count == 0) && !paused) spawn.Update(reg, trans.GetGlobalPosition());
					spawn.Render(trans.GetModel());
				}
			);

			//draw all players
			//int temp = 2;
			playerView.each(
				[&](Player& p, PhysBody& body, Transform& trans) {
					if (count == 0 && !paused) p.Update(body);
					p.Draw(trans.GetModel(), count, numOfCams, paused);
					/*if (p.IsPlayer())
						Rendering::LightsPos[temp++] = trans.GetGlobalPosition();*/
				}
			);


			//draw scene specific stuff
			if (hitboxes != nullptr) hitboxes->Render();
			if (effects != nullptr) effects->Render();

			//do all the draws
			ObjLoader::PerformDraw(view, camCam,
				DefaultColour, LightsPos, LightsColour, LightCount,
				1, 4, 0.0f, AmbientColour, AmbientStrength);
			ObjMorphLoader::PerformDraw(view, camCam,
				DefaultColour, LightsPos, LightsColour, LightCount,
				1, 4, 0.0f, AmbientColour, AmbientStrength);
			Sprite::PerformDraw();


			//map drawn first for transparency
			textObjView.each(
				[&](MultiTextObj& obj, Transform& trans) {
					obj.Draw(trans.GetModel(), view, camCam,
						DefaultColour, LightsPos, LightsColour, LightCount,
						1, 4, 0.0f, AmbientColour, AmbientStrength);
				}
			);

			//exit even if some cams haven't been checked, because only the amount specified should render
			if (++count >= numOfCams)
				break;
		}
		glViewport(0, 0, BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);
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

	glm::vec4 BackColour = { 0.2f, 0.2f, 0.2f, 1.f };
	std::array<glm::vec3, MAX_LIGHTS> LightsColour = {
	   glm::vec3(200.f),
	   glm::vec3(15.f, 15.f, 0.f),
	   glm::vec3(.5f), glm::vec3(.5f), glm::vec3(.5f), glm::vec3(.5f),
	   glm::vec3(.5f), glm::vec3(.5f), glm::vec3(.5f), glm::vec3(.5f)
	};
	std::array<glm::vec3, MAX_LIGHTS> LightsPos = {
		glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f),
		glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f),
		glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f)
	};
	size_t LightCount = 0;
	glm::vec3 DefaultColour = glm::vec3(1.f);
	glm::vec3 AmbientColour = glm::vec3(1.f);
	glm::float32 AmbientStrength = 1.f;

	HitboxGen* hitboxes = nullptr;
	Effects* effects = nullptr;
	FrameEffects* frameEffects = nullptr;
	Camera orthoVP;
}