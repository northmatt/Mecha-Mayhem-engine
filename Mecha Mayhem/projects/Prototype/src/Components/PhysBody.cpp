#include "PhysBody.h"
#include "Utilities/BLM.h"

btAlignedObjectArray<btCollisionShape*> PhysBody::m_collisionShapes = {};
btDiscreteDynamicsWorld* PhysBody::m_world = nullptr;

void PhysBody::Init(btDiscreteDynamicsWorld *world)
{
    m_world = world;
}

void PhysBody::Unload()
{
    m_world = nullptr;
}

PhysBody& PhysBody::Init(float width, float height, float depth, glm::vec3 pos, float mass, bool isDynamic)
{
    bool shapeExists = false;
    btVector3 dimensions(btScalar(width / 2), btScalar(height / 2), btScalar(depth / 2));

    btCollisionShape* boxShape = nullptr;
    //tests to see if the shape already exists
    /*for (int i(0); i < m_collisionShapes.size(); ++i) {
        if (m_collisionShapes[i]->)
    }*/

    if (!shapeExists) {
        boxShape = new btBoxShape(dimensions);

        m_collisionShapes.push_back(boxShape);
    }
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(BLM::GLMtoBT(pos));


    btVector3 localInertia(0, 0, 0);

    //when mass is zero, we make non dynamic
    if (m_dynamic = (isDynamic && mass != 0))
        boxShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, boxShape, localInertia);
    m_body = new btRigidBody(rbInfo);

    m_world->addRigidBody(m_body);

    return *this;
}

PhysBody& PhysBody::Init(float radius, float height, glm::vec3 pos, float mass, bool isDynamic)
{
    bool shapeExists = false;

    btCollisionShape* pillShape = nullptr;
    //tests to see if the shape already exists
    /*for (int i(0); i < m_collisionShapes.size(); ++i) {
        if (m_collisionShapes[i]->)
    }*/

    if (!shapeExists) {
        pillShape = new btCapsuleShape(radius, height / 0.5f);

        m_collisionShapes.push_back(pillShape);
    }
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(BLM::GLMtoBT(pos));


    btVector3 localInertia(0, 0, 0);

    //when mass is zero, we make non dynamic
    if (m_dynamic = (isDynamic && mass != 0))
        pillShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, pillShape, localInertia);
    m_body = new btRigidBody(rbInfo);

    m_world->addRigidBody(m_body);

    return *this;
}

PhysBody& PhysBody::Init(float radius, glm::vec3 pos, float mass, bool isDynamic)
{
    bool shapeExists = false;

    btCollisionShape* sphereShape = nullptr;
    //tests to see if the shape already exists
    /*for (int i(0); i < m_collisionShapes.size(); ++i) {
        if (m_collisionShapes[i]->)
    }*/

    if (!shapeExists) {
        sphereShape = new btSphereShape(radius);

        m_collisionShapes.push_back(sphereShape);
    }
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(BLM::GLMtoBT(pos));


    btVector3 localInertia(0, 0, 0);

    //when mass is zero, we make non dynamic
    if (m_dynamic = (isDynamic && mass != 0))
        sphereShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, sphereShape, localInertia);
    m_body = new btRigidBody(rbInfo);

    m_world->addRigidBody(m_body);

    return *this;
}


PhysBody& PhysBody::SetGravity(glm::vec3 grav)
{
    if (m_body)
        m_body->setGravity(BLM::GLMtoBT(grav));
    

    return *this;
}

PhysBody& PhysBody::SetGravity(btVector3 grav)
{
    if (m_body)
        m_body->setGravity(grav);
    

    return *this;
}

PhysBody& PhysBody::SetVelocity(glm::vec3 vel)
{
    if (m_body)
        m_body->setLinearVelocity(BLM::GLMtoBT(vel));

    return *this;
}

PhysBody& PhysBody::SetVelocity(btVector3 vel)
{
    if (m_body)
        m_body->setLinearVelocity(vel);

    return *this;
}

PhysBody& PhysBody::SetPosition(glm::vec3 pos)
{
    if (m_body)
        m_body->getWorldTransform().setOrigin(BLM::GLMtoBT(pos));
    m_changed = true;

    return *this;
}

PhysBody& PhysBody::SetPosition(btVector3 pos)
{
    if (m_body)
        m_body->getWorldTransform().setOrigin(pos);
    m_changed = true;

    return *this;
}

PhysBody& PhysBody::SetRotation(glm::quat rot)
{
    if (m_body)
        m_body->getWorldTransform().setRotation(BLM::GLMtoBT(rot));
    m_changed = true;

    return *this;
}

PhysBody& PhysBody::SetRotation(btQuaternion rot)
{
    if (m_body)
        m_body->getWorldTransform().setRotation(rot);
    m_changed = true;

    return *this;
}

PhysBody& PhysBody::SetAwake(bool isAwake)
{
    if (m_body)
        m_body->activate(isAwake);
    m_changed = true;

    return *this;
}

btTransform PhysBody::GetTransform()
{
    //will assume you only get transform for the transform component update
    m_changed = false;

    return m_body->getWorldTransform();
}
