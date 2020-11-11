#include "Rendering.h"

namespace Rendering {

    void Update(entt::registry* reg, int numOfCams)
    {
        glClearColor(BackColour.x, BackColour.y, BackColour.z, BackColour.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto objView = reg->view<ObjLoader, Transform>();
        auto cameraView = reg->view<Camera, Transform>();

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

            LightPos = camCam.SetPosition(camTrans.GetGlobalPosition()).
                SetForward(camTrans.GetForwards()).
                GetPosition();

            ObjLoader::BeginDraw();

            //draw all the objs
            for (auto entity : objView)
            {
                Transform& trans = objView.get<Transform>(entity);

                objView.get<ObjLoader>(entity).Draw(entity, trans.GetModel());
            }

            ObjLoader::PerformDraw(view, camCam, DefaultColour, LightPos, LightColour, 1, 4, 0.1f);
            ++count;
            //exit even if some cams haven't been checked, because only the amount specified should render
            if (count > numOfCams)
                break;
        }
    }

    glm::vec4 BackColour = { 0.2f, 0.2f, 0.2f, 1.f };
    glm::vec3 LightColour = glm::vec3(3.f);
    glm::vec3 LightPos = glm::vec3(0.f);
    glm::vec3 DefaultColour = glm::vec3(1.f);

}