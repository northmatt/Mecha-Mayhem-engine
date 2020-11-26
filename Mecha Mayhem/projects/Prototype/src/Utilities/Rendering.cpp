#include "Rendering.h"

namespace Rendering {

    void Update(entt::registry* reg, int numOfCams, float dt)
    {
        glClearColor(BackColour.x, BackColour.y, BackColour.z, BackColour.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto& objView = reg->view<ObjLoader, Transform>();
        auto& morphView = reg->view<ObjMorphLoader, Transform>();
        auto& cameraView = reg->view<Camera, Transform>();

        int height = BackEnd::GetHalfHeight();
        int width = BackEnd::GetHalfWidth();
        short count = 1;
        for (auto cam : cameraView)
        {
            //glViewport((count % 2 == 0 ? width : 0), ((count < 3) && (numOfCams > 2) ? height : 0),
            //    width * (numOfCams == 1 ? 2 : 1), height * (numOfCams > 2 ? 1 : 2));

            if (numOfCams > 2)
                glViewport((count % 2 == 1 ? 0 : width), (count < 3 ? height : 0), width, height);
            else if (numOfCams == 2)
                glViewport((count == 1 ? 0 : width), 0, width, height * 2);
            else
                glViewport(0, 0, width * 2, height * 2);

            Transform& camTrans = cameraView.get<Transform>(cam);
            glm::mat4 view = glm::inverse(camTrans.GetModel());

            Camera& camCam = cameraView.get<Camera>(cam);

            camCam.SetPosition(camTrans.GetGlobalPosition()).
                SetForward(camTrans.GetForwards());

            ObjLoader::BeginDraw(objView.size());

            //draw all the objs
            for (auto entity : objView)
            {
                Transform& trans = objView.get<Transform>(entity);

                objView.get<ObjLoader>(entity).Draw(trans.GetModel());
            }

            if (hitboxes != nullptr) hitboxes->Render();

            ObjLoader::PerformDraw(view, camCam, DefaultColour, LightPos, LightColour, 1, 4, 0.5f);

            ObjMorphLoader::BeginDraw(morphView.size());

            //draw all the objs
            for (auto entity : morphView)
            {
                Transform& trans = morphView.get<Transform>(entity);

                if (count == 1)
                    morphView.get<ObjMorphLoader>(entity).Update(dt);
                morphView.get<ObjMorphLoader>(entity).Draw(trans.GetModel());
            }

            ObjMorphLoader::PerformDraw(view, camCam, DefaultColour, LightPos, LightColour, 1, 4, 0.5f);

            ++count;
            //exit even if some cams haven't been checked, because only the amount specified should render
            if (count > numOfCams)
                break;
        }
    }

    glm::vec4 BackColour = { 0.2f, 0.2f, 0.2f, 1.f };
    glm::vec3 LightColour[MAX_LIGHTS] = { glm::vec3(3.f) };
    glm::vec3 LightPos[MAX_LIGHTS] = { glm::vec3(0.f) };
    glm::vec3 DefaultColour = glm::vec3(1.f);
    size_t LightCount = 2;

    HitboxGen* hitboxes = nullptr;
}