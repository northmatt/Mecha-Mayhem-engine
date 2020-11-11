#include "Scene.h"

btDbvtBroadphase* Scene::_broadphase = nullptr;
btCollisionConfiguration * Scene::_collisionConfiguration = nullptr;
btCollisionDispatcher * Scene::_dispatcher = nullptr;
btSequentialImpulseConstraintSolver * Scene::_solver = nullptr;

Scene::Scene(const std::string& name, const glm::vec3& gravity, bool physics)
    : m_name(name)
{
    if (physics) {
        m_world = new btDiscreteDynamicsWorld(
            _dispatcher, _broadphase, _solver, _collisionConfiguration);

        m_world->setGravity(BLM::GLMtoBT(gravity));
    }
}

Scene::~Scene()
{
    delete m_world;
}

entt::registry* Scene::GetRegistry()
{
    return &m_reg;
}

std::string Scene::GetName()
{
    return m_name;
}

void Scene::Init(bool causeOverload)
{
    _broadphase = new btDbvtBroadphase();
    _collisionConfiguration = new btDefaultCollisionConfiguration();
    _dispatcher = new btCollisionDispatcher(_collisionConfiguration);
    _solver = new btSequentialImpulseConstraintSolver();
}

void Scene::Unload(bool causeOverload)
{
    _broadphase = nullptr;
    _collisionConfiguration = nullptr;
    _dispatcher = nullptr;
    _solver = nullptr;
}

void Scene::Init(int windowWidth, int windowHeight)
{
    //generic init, use as guidelines
    ECS::AttachRegistry(&m_reg);
    if (m_world)
        PhysBody::Init(m_world);

    // for multi cam setup, change the m_camCount variable, and also spawn in reverse order (aka player 1 last)
    unsigned cameraEnt = ECS::CreateEntity();
    auto& camCam = ECS::AttachComponent<Camera>(cameraEnt);
    camCam.SetFovDegrees(60.f).ResizeWindow(windowWidth, windowHeight);
}

void Scene::BackEndUpdate()
{
    m_dt = glfwGetTime() - m_lastClock;
    m_lastClock = glfwGetTime();

    if (m_world != nullptr) {

        m_world->stepSimulation(m_dt, 10);

        auto physView = m_reg.view<PhysBody, Transform>();

        for (auto entity : physView) {

            auto phys = m_reg.get<PhysBody>(entity);

            if (phys.IsDynamic() || phys.Changed()) {
                m_reg.get<Transform>(entity).SetTransform(
                    m_reg.get<PhysBody>(entity).GetTransform()
                );
            }
        }
    }

    //auto transView = m_reg.view<Transform>();

    //for (auto entity : transView) {
    //    //recalculate if has parent
    //    if (transView.get(entity).HasParent()) {
    //        m_reg.get<Transform>(entity).ComputeGlobal();
    //    }
    //}

    //always render
    if (m_camCount < 1)
        m_camCount = 1;
    else if (m_camCount > 4)
        m_camCount = 4;

    Rendering::Update(&m_reg, m_camCount);
}
