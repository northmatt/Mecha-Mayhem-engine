#include "ECS.h"

entt::registry* ECS::m_registry = nullptr;
btDiscreteDynamicsWorld* ECS::m_world = nullptr;

void ECS::AttachRegistry(entt::registry* reg)
{
    m_registry = reg;
}

void ECS::AttachWorld(btDiscreteDynamicsWorld* world)
{
    m_world = world;
}

void ECS::DettachRegistry()
{
    m_registry = nullptr;
}

entt::registry* ECS::GetRegistry()
{
    return m_registry;
}

unsigned ECS::CreateEntity(bool hasPhysics)
{
    unsigned entity = m_registry->create();

    if (hasPhysics)
        m_registry->assign<Transform>(entity);

    return entity;
}

void ECS::DestroyEntity(entt::entity entity)
{
    if (m_registry->has<PhysBody>(entity)) {
        m_world->removeRigidBody(m_registry->get<PhysBody>(entity).GetBody());
    }
    m_registry->destroy(entity);
}

bool ECS::Exists(entt::entity entity)
{
    return m_registry->valid(entity);
}
