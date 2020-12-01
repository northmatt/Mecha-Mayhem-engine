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

PhysBody& PhysBody::Init(int id, float width, float height, float depth, glm::vec3 pos, float mass, bool isDynamic)
{
    bool shapeExists = false;
    btVector3 dimensions(btScalar(width / 2), btScalar(height / 2), btScalar(depth / 2));

    btCollisionShape* boxShape = nullptr;
    //tests to see if the shape already exists
    /*for (int i(0); i < m_collisionShapes.size(); ++i) {
        if (m_collisionShapes[i]->) {
            shapeExists = true;
            boxShape = m_collisionShapes[i];
            break;
        }
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
    m_body->setUserIndex(id);

    m_world->addRigidBody(m_body);

    return *this;
}

PhysBody& PhysBody::Init(int id, float radius, float height, glm::vec3 pos, float mass, bool isDynamic)
{
    bool shapeExists = false;

    btCollisionShape* pillShape = nullptr;
    /*for (int i(0); i < m_collisionShapes.size(); ++i) {
        if (m_collisionShapes[i]->) {
            shapeExists = true;
            pillShape = m_collisionShapes[i];
            break;
        }
    }*/

    if (!shapeExists) {
        pillShape = new btCapsuleShape(radius, height - radius * 2);

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
    m_body->setUserIndex(id);

    m_world->addRigidBody(m_body);

    return *this;
}

PhysBody& PhysBody::Init(int id, float radius, glm::vec3 pos, float mass, bool isDynamic)
{
    bool shapeExists = false;

    btCollisionShape* sphereShape = nullptr;
    /*for (int i(0); i < m_collisionShapes.size(); ++i) {
        if (std::string(m_collisionShapes[i]->getName())) {
            shapeExists = true;
            boxShape = m_collisionShapes[i];
            break;
        }
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
    m_body->setUserIndex(id);

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

RayResult PhysBody::GetRaycastResult(btVector3 from, btVector3 to)
{
    if (m_world) {
        RayResult closestResults(from, to);

        m_world->rayTest(from, to, closestResults);

        return closestResults;
    }
}

btVector3 PhysBody::GetRaycastWithDistanceLimit(glm::vec3 startPos, glm::vec3 look, float limit)
{
    if (m_world)
    {
        ///first hit
        {
            //player position converstion to bullet vector3
            btVector3 from = BLM::GLMtoBT(startPos);
            btVector3 to = BLM::GLMtoBT(look);

            RayResult closestResults = GetRaycastResult(from, to);

            //if it hits it runs this code
            if (closestResults.hasHit())
            {
                //this puts the coordinate in which it hit
                btVector3 p = closestResults.m_hitPointWorld - from;
                if (p.length() > limit)
                    return from.lerp(from + to.normalize(), limit);
                else
                    return closestResults.m_hitPointWorld;
            }
            else
            {
                return from.lerp(from + to.normalize(), limit);
            }
        }
    }
}

btVector3 PhysBody::GetRaycast(glm::vec3 startPos, glm::vec3 look)
{
    if (m_world)
    {
        ///first hit
        {
            //player position converstion to bullet vector3
            btVector3 from = BLM::GLMtoBT(startPos);
            btVector3 to = BLM::GLMtoBT(look);

            RayResult closestResults = GetRaycastResult(from, to);

            //if it hits it runs this code
            if (closestResults.hasHit())
            {
                //this puts the coordinate in which it hit
                btVector3 p = from.lerp(from + to, closestResults.m_closestHitFraction);
                return p;
            }
        }
    }
}

btVector3 PhysBody::GetRaycast(glm::vec3 look)
{
    if (m_world)
    {
        ///first hit
        {
            //player position converstion to bullet vector3
            btVector3 from = m_body->getWorldTransform().getOrigin();
            btVector3 to = BLM::GLMtoBT(look);

            RayResult closestResults = GetRaycastResult(from, to);

            //if it hits it runs this code
            if (closestResults.hasHit())
            {
                //this puts the coordinate in which it hit
                btVector3 p = from.lerp(from + to, closestResults.m_closestHitFraction);
                return p;

                //debugging purposes
              //      std::cout << p.x() << "," << p.y() << "," << p.z() << std::endl;
            }
        }
    }
}

