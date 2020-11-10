#include "Rendering.h"

namespace Rendering {

    void Update(entt::registry* reg)
    {
        glClearColor(BackColour.x, BackColour.y, BackColour.z, BackColour.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto objView = reg->view<ObjLoader, Transform>();
        auto cameraView = reg->view<Camera, Transform>();

        for (auto cam : cameraView)
        {
            glm::mat4 view = glm::inverse(cameraView.get<Transform>(cam).ComputeGlobal().GetModel());

            Camera& camCam = cameraView.get<Camera>(cam);

            LightPos = cameraView.get<Transform>(cam).GetGlobalPosition();

            ObjLoader::BeginDraw();

            //draw all the objs
            for (auto entity : objView)
            {
                Transform& trans = objView.get<Transform>(entity);

                objView.get<ObjLoader>(entity).Draw(entity, trans.GetModel());
            }

            ObjLoader::PerformDraw(view, camCam, DefaultColour, LightPos, LightColour);
        }
    }

    glm::vec4 BackColour = { 0.2f, 0.2f, 0.2f, 1.f };
    glm::vec3 LightColour = glm::vec3(3.f);
    glm::vec3 LightPos = glm::vec3(0.f);
    glm::vec3 DefaultColour = glm::vec3(1.f);

}