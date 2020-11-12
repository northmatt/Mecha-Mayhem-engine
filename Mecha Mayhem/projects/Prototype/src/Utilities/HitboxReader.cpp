#include "HitboxReader.h"
#include "Input.h"
#include "BLM.h"

ObjLoader HitboxGen::m_cube;
ObjLoader HitboxGen::m_cubeCurrent;
btCollisionShape* HitboxGen::m_planeShape = nullptr;
Transform HitboxGen::m_defaultTrans;

bool HitboxGen::Init(btDiscreteDynamicsWorld* world, const std::string& filename, bool newFile, bool overwrite)
{
    if (m_world != nullptr) {
        for (int i(0); i < m_objects.size(); ++i) {
            m_world->removeRigidBody(m_objects[i].body);
        }
        m_objects.resize(0);
        m_boxShape.resize(0);
        m_world->removeRigidBody(m_floor);
    }
    m_world = world;
    m_filename = "maps/" + filename;

    //if overwrite, truncate
    std::fstream file;
    if (overwrite)      file.open(m_filename, std::ios::trunc);
    else                file.open(m_filename);

    if (!file) {            //if file doesn't exists, overwrite means nothing, so check if new file
        if (newFile) {
            std::cout << "creating " + m_filename + "\n";
            file.open(m_filename, std::fstream::out);
            file.close();
        }
        else {
            m_filename = "";
            return false;
        }

        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, m_planeShape);
        m_floor = new btRigidBody(rbInfo);
        m_world->addRigidBody(m_floor);
    }
    else if (overwrite) {
        std::cout << "overwriting existing file" + m_filename + "\n";

        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, m_planeShape);
        m_floor = new btRigidBody(rbInfo);
        m_world->addRigidBody(m_floor);
    }
    else {
        if (newFile && !overwrite) {
            while (file) {
                m_filename += "1";
                file.close();
                file.open(m_filename);
            }
            file.open(m_filename, std::fstream::out);
            file.close();

            std::cout << "found an existing file, newfile specified, so \"" + m_filename + "\" created\n";

            btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, m_planeShape);
            m_floor = new btRigidBody(rbInfo);
            m_world->addRigidBody(m_floor);
        }
        else {
            std::cout << "found the file, reading data\n";

            btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, m_planeShape);
            m_floor = new btRigidBody(rbInfo);
            m_world->addRigidBody(m_floor);

            std::string line;
            while (std::getline(file, line)) {
                std::istringstream ss(line);
                std::string input;
                ss >> input;
                if (input == "box") {
                    glm::vec3 pos(0.f);
                    glm::quat rot(1.f, 0.f, 0.f, 0.f);
                    glm::vec3 scl(1.f);
                    while (!ss.eof()) {
                        ss >> input;
                        if (input == "pos")         ss >> pos.x >> pos.y >> pos.z;
                        else if (input == "rot")    ss >> rot.x >> rot.y >> rot.z >> rot.w;
                        else if (input == "scl")    ss >> scl.x >> scl.y >> scl.z;
                    }


                    btCollisionShape* tempBox = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
                    m_boxShape.push_back(tempBox);
                    btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempBox);
                    btRigidBody* tempBod = new btRigidBody(rbInfo);

                    tempBod->getWorldTransform() = btTransform(BLM::GLMtoBT(rot), BLM::GLMtoBT(pos));
                    tempBod->getCollisionShape()->setLocalScaling(BLM::GLMtoBT(scl));

                    m_world->addRigidBody(tempBod);

                    m_objects.push_back({ Transform(pos, rot, scl), tempBod });
                }
                else if (input == "floor") {
                    float height = 0;
                    ss >> height;
                    m_floor->getWorldTransform().getOrigin().setY(height);
                }
            }

        }
    }
    file.close();

    if (m_current >= m_objects.size())  m_current = m_objects.size() - 1;
    if (m_objects.size() == 0)          m_current = 0;

    return true;
}

bool HitboxGen::SaveToFile(bool overwriteExisting)
{
    if (m_filename == "") {
        std::cout << "no file loaded, so no save\n";
        return true;
    }
    if (!m_world)   return false;

    std::fstream file(m_filename);

    if (!file)      return false;

    if (overwriteExisting) {
        file.close();
        file.open(m_filename, std::ios::out);
        std::cout << "overwriting existing file\n";
    }
    else if (file >> std::string()) {
        file.close();
        std::cout << "data in file, won't overwrite\n";
        return true;
    }

    float height = m_floor->getWorldTransform().getOrigin().y();
    if (height != 0)        file << "floor " << height;

    for (short i(0); i < m_objects.size(); ++i) {
        file << "\n" << "box ";
        auto trans = m_objects[i].trans;
        glm::vec3 pos = trans.GetPosition();
        if (glm::length(pos) != 0)
            file << "pos " << pos.x << " " << pos.y << " " << pos.z << " ";

        glm::quat rot = trans.GetRotation();
        if (rot.w != 1 || rot.x != 0 || rot.y != 0 || rot.z != 0)
            file << "rot " << rot.x << " " << rot.y << " " << rot.z << " " << rot.w << " ";

        glm::vec3 scl = trans.GetScale();
        if (scl.x != 1 || scl.y != 1 || scl.z != 1)
            file << "scl " << scl.x << " " << scl.y << " " << scl.z << " ";
    }
    std::cout << "saved data\n";

    file.close();
    return true;
}

bool HitboxGen::LoadFromFile()
{
    if (m_filename == "") {
        std::cout << "no file loaded, so no save\n";
        return true;
    }
    if (!m_world)   return false;

    std::fstream file(m_filename);

    if (!file)      return false;
    std::cout << "found the file, replacing data\n";
    for (int i(0); i < m_objects.size(); ++i) {
        m_world->removeRigidBody(m_objects[i].body);
    }
    m_objects.resize(0);
    m_boxShape.resize(0);
    m_world->removeRigidBody(m_floor);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, m_planeShape);
    m_floor = new btRigidBody(rbInfo);
    m_world->addRigidBody(m_floor);

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string input;
        ss >> input;
        if (input == "box") {
            glm::vec3 pos(0.f);
            glm::quat rot(1.f, 0.f, 0.f, 0.f);
            glm::vec3 scl(1.f);
            while (!ss.eof()) {
                ss >> input;
                if (input == "pos")         ss >> pos.x >> pos.y >> pos.z;
                else if (input == "rot")    ss >> rot.x >> rot.y >> rot.z >> rot.w;
                else if (input == "scl")    ss >> scl.x >> scl.y >> scl.z;
            }

            btCollisionShape* tempBox = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
            m_boxShape.push_back(tempBox);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempBox);
            btRigidBody* tempBod = new btRigidBody(rbInfo);

            tempBod->getWorldTransform() = btTransform(BLM::GLMtoBT(rot), BLM::GLMtoBT(pos));
            tempBod->getCollisionShape()->setLocalScaling(BLM::GLMtoBT(scl));

            m_world->addRigidBody(tempBod);
            m_objects.push_back({ Transform(pos, rot, scl), tempBod });
        }
        else if (input == "floor") {
            float height = 0;
            ss >> height;
            m_floor->getWorldTransform().getOrigin().setY(height);
        }
    }

    if (m_current >= m_objects.size())  m_current = m_objects.size() - 1;
    if (m_objects.size() == 0)          m_current = 0;

    return true;
}

void HitboxGen::Render()
{
    if (!m_world || !m_draw)    return;

    if (m_floor) {
        Transform floor;
        floor.SetPosition(m_floor->getWorldTransform().getOrigin()).SetScale(glm::vec3(1000, 0, 1000));
        m_cube.Draw(0, floor.GetModel());
    }

    for (short i(0); i < m_objects.size(); ++i) {
        if (i == m_current)     m_cubeCurrent.Draw(0, m_objects[i].trans.GetModel());
        else                    m_cube.Draw(0, m_objects[i].trans.GetModel());
    }
}

void HitboxGen::Update(float dt)
{
    if (!m_world)   return;

    if (Input::GetKey(KEY::TAB))    dt = 0.1f * dt;

    {
        float speed = 0;
        btVector3& trans = m_floor->getWorldTransform().getOrigin();
        if (Input::GetKey(KEY::LBRACKET))   speed -= (Input::GetKey(KEY::LCTRL) ? dt : 5 * dt);
        if (Input::GetKey(KEY::RBRACKET))   speed += (Input::GetKey(KEY::LCTRL) ? dt : 5 * dt);
        if (speed != 0)     trans.setY(speed = trans.y() + speed);
    }

    if (!m_draw)    return;

    if (Input::GetKeyDown(KEY::N)) {
        btCollisionShape* tempBox = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
        m_boxShape.push_back(tempBox);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempBox);
        btRigidBody* tempBod = new btRigidBody(rbInfo);
        m_world->addRigidBody(tempBod);

        m_objects.push_back({ m_defaultTrans, tempBod });
    }

    if (m_objects.size() == 0) return;

    if (Input::GetKeyDown(KEY::B)) {
        m_world->removeRigidBody(m_objects[m_current].body);
        m_objects.erase(m_objects.begin() + m_current);
        m_boxShape.removeAtIndex(m_current);
        if (m_current >= m_objects.size())  m_current = m_objects.size() - 1;
        if (m_objects.size() == 0) { m_current = 0; return; }
    }
    if (Input::GetKeyDown(KEY::M)) {
        btCollisionShape* tempBox = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
        m_boxShape.push_back(tempBox);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempBox);
        btRigidBody* tempBod = new btRigidBody(rbInfo);
        tempBod->getWorldTransform() = m_objects[m_current].body->getWorldTransform();
        tempBod->getCollisionShape()->setLocalScaling(
            BLM::GLMtoBT(m_objects[m_current].trans.GetScale()));

        m_world->addRigidBody(tempBod);
        m_objects.push_back({ m_objects[m_current].trans, tempBod });
    }

    if (Input::GetKeyDown(KEY::Y))
        if (m_current-- == 0) m_current = m_objects.size() - 1;
    if (Input::GetKeyDown(KEY::U))
        if (++m_current == m_objects.size()) m_current = 0;

    //transformations
    {
        auto& currentTrans = m_objects[m_current].trans;

        float speed = (Input::GetKey(KEY::LCTRL) ? dt : 15 * dt);

        glm::vec3 change = glm::vec3(0.f);
        if (Input::GetKey(KEY::I))      change.z -= speed;
        if (Input::GetKey(KEY::K))      change.z += speed;
        if (Input::GetKey(KEY::J))      change.x -= speed;
        if (Input::GetKey(KEY::L))      change.x += speed;
        if (Input::GetKey(KEY::PERIOD)) change.y += speed;
        if (Input::GetKey(KEY::COMMA))  change.y -= speed;
        if (change.x != 0 || change.z != 0 || change.y != 0) {
            m_objects[m_current].body->getWorldTransform().
                setOrigin(BLM::GLMtoBT(currentTrans.SetPosition(
                    currentTrans.GetPosition() +
                    glm::rotate(currentTrans.GetRotation(), change)
                ).GetPosition())
            );
            change = glm::vec3(0);
        }
        if (Input::GetKeyDown(KEY::T))      change.z += glm::radians(45.f);
        if (Input::GetKeyDown(KEY::F))      change.x -= glm::radians(45.f);
        if (Input::GetKeyDown(KEY::G))      change.z -= glm::radians(45.f);
        if (Input::GetKeyDown(KEY::H))      change.x += glm::radians(45.f);
        if (change.x != 0 || change.z != 0) {
            m_objects[m_current].body->getWorldTransform().setRotation(BLM::GLMtoBT(
                currentTrans.SetRotation(currentTrans.GetRotation() *
                    glm::angleAxis(change.z, glm::vec3(1, 0, 0)) *
                    glm::angleAxis(change.x, glm::vec3(0, 1, 0))
                ).GetRotation())
            );
            change = glm::vec3(0);
        }

        if (Input::GetKey(KEY::SEVEN))      change.x += dt;
        if (Input::GetKey(KEY::EIGHT))      change.y += dt;
        if (Input::GetKey(KEY::NINE))       change.z += dt;
        if (change.x != 0 || change.y != 0 || change.z != 0) {
            glm::vec3 scale = currentTrans.GetScale() +
                (Input::GetKey(KEY::LCTRL) ? -change : change);
            scale.x = glm::clamp(scale.x, 1.f, 1000.f);
            scale.y = glm::clamp(scale.y, 1.f, 1000.f);
            scale.z = glm::clamp(scale.z, 1.f, 1000.f);

            currentTrans.SetScale(scale);
            m_objects[m_current].body->getCollisionShape()->
                setLocalScaling(BLM::GLMtoBT(scale));
        }
    }
}
