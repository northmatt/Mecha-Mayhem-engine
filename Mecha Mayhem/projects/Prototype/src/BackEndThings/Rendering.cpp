#include "Rendering.h"

void Rendering::Update(entt::registry* reg)
{
    glClearColor(BackColour.x, BackColour.y, BackColour.z, BackColour.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto objView = reg->view<ObjLoader, Transform>();
    auto cameraView = reg->view<Camera, Transform>();

    for (auto cam : cameraView)
    {
        cameraView.get<Camera>(cam).SetPosition(cameraView.get<Transform>(cam).GetPosition());
        cameraView.get<Camera>(cam).SetRotation(cameraView.get<Transform>(cam).GetRotation());

        //draw all the objs
        for (auto entity : objView)
        {
            Transform &trans = objView.get<Transform>(entity);

            objView.get<ObjLoader>(entity).Draw(
                cameraView.get<Camera>(cam), trans.GetModel(), trans.GetRotationM3(),
                glm::vec3(1.f), cameraView.get<Transform>(cam).GetPosition(), glm::vec3(3.f));
        }
    }
}
