#include "HitboxReader.h"
#include "Input.h"
#include "Components/ECS.h"

ObjLoader HitboxGen::m_cube;
ObjLoader HitboxGen::m_cylinder;
ObjLoader HitboxGen::m_cubeCurrent;
ObjLoader HitboxGen::m_cylinderCurrent;
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
	m_filename = filename;

	//if overwrite, truncate
	std::fstream file;
	if (overwrite)      file.open("maps/" + m_filename + ".jpegs", std::ios::trunc);
	else                file.open("maps/" + m_filename + ".jpegs");

	if (!file) {            //if file doesn't exists, overwrite means nothing, so check if new file
		if (newFile) {
			std::cout << "creating \"maps/" + m_filename + ".jpegs\"\n";
			file.open("maps/" + m_filename + ".jpegs", std::fstream::out);
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
		std::cout << "overwriting existing file \"maps/" + m_filename + ".jpegs\"\n";

		btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, m_planeShape);
		m_floor = new btRigidBody(rbInfo);
		m_world->addRigidBody(m_floor);
	}
	else {
		if (newFile && !overwrite) {
			while (file) {
				size_t index = m_filename.size() - 1;
				if (std::isdigit(m_filename[index])) {
					if (m_filename[index] < '9')
						++m_filename[index];
					else if (index > 0) {
						while (index >= 0) {
							if (!std::isdigit(m_filename[index]))
								break;
							if (index > 0)    --index;
						}
						int num = std::stoi(m_filename.substr(++index));
						m_filename = m_filename.substr(0, index) + std::to_string(++num);
					}
					else {
						m_filename[index] = '1';
						m_filename += "0";
					}
						
				}
				else {
					m_filename += "0";
				}
				file.close();
				file.open("maps/" + m_filename + ".jpegs");
			}
			file.open("maps/" + m_filename + ".jpegs", std::fstream::out);

			std::cout << "found an existing file, newfile specified, so \"maps/" + m_filename + ".jpegs\" created\n";

			btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, m_planeShape);
			m_floor = new btRigidBody(rbInfo);
			m_world->addRigidBody(m_floor);
		}
		else {
			std::cout << "found \"maps/" + m_filename + ".jpegs\", reading data\n";

			btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, m_planeShape);
			m_floor = new btRigidBody(rbInfo);
			m_world->addRigidBody(m_floor);

			std::string line;
			while (std::getline(file, line)) {
				std::istringstream ss(line);
				std::string input;
				ss >> input;
				if (input == "box" || input == "cyl") {
					std::string typeHold = input;

					glm::vec3 pos(0.f);
					glm::quat rot(1.f, 0.f, 0.f, 0.f);
					glm::vec3 scl(1.f);
					while (!ss.eof()) {
						ss >> input;
						if (input == "pos")         ss >> pos.x >> pos.y >> pos.z;
						else if (input == "rot")    ss >> rot.x >> rot.y >> rot.z >> rot.w;
						else if (input == "scl")    ss >> scl.x >> scl.y >> scl.z;
					}

					btCollisionShape* tempShape = nullptr;
					if (typeHold == "box")		tempShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
					else if (typeHold == "cyl")	tempShape = new btCylinderShape(btVector3(0.5f, 0.5f, 0.5f));
					if (tempShape == nullptr)	throw std::runtime_error("shape machine broke somehow");
					m_boxShape.push_back(tempShape);
					btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempShape);
					btRigidBody* tempBod = new btRigidBody(rbInfo);

					tempBod->getWorldTransform() = btTransform(BLM::GLMtoBT(rot), BLM::GLMtoBT(pos));
					tempBod->getCollisionShape()->setLocalScaling(BLM::GLMtoBT(scl));

					m_world->addRigidBody(tempBod);

					m_objects.push_back({ Transform(pos, rot, scl), tempBod, typeHold });
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

	std::fstream file("maps/" + m_filename + ".jpegs");

	if (!file)      return false;

	if (overwriteExisting) {
		file.close();
		file.open("maps/" + m_filename + ".jpegs", std::ios::out);
		std::cout << "overwriting \"maps/" + m_filename + ".jpegs\", ";
	}
	else if (file >> std::string()) {
		std::string oldName = m_filename;
		while (file) {
			size_t index = m_filename.size() - 1;
			if (std::isdigit(m_filename[index])) {
				if (m_filename[index] < '9')
					++m_filename[index];
				else if (index > 0) {
					while (index >= 0) {
						if (!std::isdigit(m_filename[index]))
							break;
						if (index > 0)    --index;
					}
					int num = std::stoi(m_filename.substr(++index));
					m_filename = m_filename.substr(0, index) + std::to_string(++num);
				}
				else {
					m_filename[index] = '1';
					m_filename += "0";
				}

			}
			else {
				m_filename += "0";
			}
			file.close();
			file.open("maps/" + m_filename + ".jpegs");
		}
		file.open("maps/" + m_filename + ".jpegs", std::fstream::out);

		std::cout << "\"maps/" + oldName + ".jpegs\" has data, so \"maps/" + m_filename + ".jpegs\" created, ";
	}
	else {
		file.close();
		file.open(m_filename, std::fstream::out);
	}

	float height = m_floor->getWorldTransform().getOrigin().y();
	if(height != 0)     file << "floor " << height;

	for (short i(0); i < m_objects.size(); ++i) {
		file << "\n" << m_objects[i].type;
		auto trans = m_objects[i].trans;
		glm::vec3 pos = trans.GetLocalPosition();
		if (glm::length(pos) != 0)
			file << " pos " << pos.x << ' ' << pos.y << ' ' << pos.z;

		glm::quat rot = trans.GetLocalRotation();
		if (rot.w != 1 || rot.x != 0 || rot.y != 0 || rot.z != 0)
			file << " rot " << rot.x << ' ' << rot.y << ' ' << rot.z << ' ' << rot.w;

		glm::vec3 scl = trans.GetScale();
		if (scl.x != 1 || scl.y != 1 || scl.z != 1)
			file << " scl " << scl.x << ' ' << scl.y << ' ' << scl.z;
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

	std::ifstream file("maps/" + m_filename + ".jpegs");

	if (!file)      return false;
	std::cout << "found the \"maps/" + m_filename + ".jpegs\", replacing data\n";
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
		if (input == "box" || input == "cyl") {
			std::string typeHold = input;

			glm::vec3 pos(0.f);
			glm::quat rot(1.f, 0.f, 0.f, 0.f);
			glm::vec3 scl(1.f);
			while (!ss.eof()) {
				ss >> input;
				if (input == "pos")         ss >> pos.x >> pos.y >> pos.z;
				else if (input == "rot")    ss >> rot.x >> rot.y >> rot.z >> rot.w;
				else if (input == "scl")    ss >> scl.x >> scl.y >> scl.z;
			}

			btCollisionShape* tempShape = nullptr;
			if (typeHold == "box")		tempShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
			else if (typeHold == "cyl")	tempShape = new btCylinderShape(btVector3(0.5f, 0.5f, 0.5f));
			if (tempShape == nullptr)	throw std::runtime_error("shape machine broke somehow");
			m_boxShape.push_back(tempShape);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempShape);
			btRigidBody* tempBod = new btRigidBody(rbInfo);

			tempBod->getWorldTransform() = btTransform(BLM::GLMtoBT(rot), BLM::GLMtoBT(pos));
			tempBod->getCollisionShape()->setLocalScaling(BLM::GLMtoBT(scl));

			m_world->addRigidBody(tempBod);
			m_objects.push_back({ Transform(pos, rot, scl), tempBod, typeHold });
		}
		else if (input == "floor") {
			float height = 0;
			ss >> height;
			m_floor->getWorldTransform().getOrigin().setY(height);
		}
	}

	file.close();

	if (m_current >= m_objects.size())  m_current = m_objects.size() - 1;
	if (m_objects.size() == 0)          m_current = 0;

	return true;
}

void HitboxGen::Render()
{
	if (!m_world || !m_draw)    return;

	if (m_floor) {
		Transform floor;
		floor.SetPosition(m_floor->getWorldTransform().getOrigin()).SetScale(glm::vec3(100000, 0, 100000));
		m_cube.Draw(floor.GetModel());
	}

	for (short i(0); i < m_tempObjects.size(); ++i) {
		m_cubeCurrent.Draw(m_tempObjects[i].GetModel());
	}

	for (short i(0); i < m_objects.size(); ++i) {
		if (m_objects[i].type == "box") {
			if (i != m_current)
				m_cube.Draw(m_objects[i].trans.GetModel());
			else
				m_cubeCurrent.Draw(m_objects[i].trans.GetModel());
		}
		else if (m_objects[i].type == "cyl") {
			if (i != m_current)
				m_cylinder.Draw(m_objects[i].trans.GetModel());
			else
				m_cylinderCurrent.Draw(m_objects[i].trans.GetModel());
		}
	}
}

void HitboxGen::Update(entt::entity cameraEnt)
{
	if (!m_world)   return;
	if (!ImGui::CollapsingHeader("Hitbox Editor"))	return;

	if (ImGui::TreeNode("Utilities")) {

		{
			if (ImGui::InputFloat2("Floor Bounds", floorHeight, 2)) {
				floorHeight[0] = glm::clamp(floorHeight[0], -100.f, floorHeight[1]);
				floorHeight[1] = glm::clamp(floorHeight[1], floorHeight[0], 100.f);
			}
			btVector3& trans = m_floor->getWorldTransform().getOrigin();
			float pos = trans.y();
			if (ImGui::SliderFloat("Floor Height", &pos, floorHeight[0], floorHeight[1])) {
				trans.setY(pos);
			}
		}

		if (m_draw) {
			if (ImGui::Button("New Box")) {
				btCollisionShape* tempBox = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
				m_boxShape.push_back(tempBox);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempBox);
				btRigidBody* tempBod = new btRigidBody(rbInfo);
				m_world->addRigidBody(tempBod);

				m_current = m_objects.size();
				m_objects.push_back({ m_defaultTrans, tempBod, "box" });
			}

			ImGui::SameLine();

			if (ImGui::Button("New Cylinder")) {
				btCollisionShape* tempCyl = new btCylinderShape(btVector3(0.5f, 0.5f, 0.5f));
				m_boxShape.push_back(tempCyl);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempCyl);
				btRigidBody* tempBod = new btRigidBody(rbInfo);
				m_world->addRigidBody(tempBod);

				m_current = m_objects.size();
				m_objects.push_back({ m_defaultTrans, tempBod, "cyl" });
			}

			//only allow these when you have objects
			int objCount = m_objects.size();
			if (objCount > 0) {
				ImGui::SameLine();
				if (ImGui::Button("Delete Selected")) {
					m_world->removeRigidBody(m_objects[m_current].body);
					m_objects.erase(m_objects.begin() + m_current);
					m_boxShape.removeAtIndex(m_current);
					if (m_current >= objCount)  m_current = objCount - 1;
					
					//exit if size becomes 0
					if (m_objects.size() == 0) {
						m_current = 0;
						ImGui::TreePop();
						return;
					}
				}

				if (objCount > 10)
					ImGui::SliderInt("Selected min", &selectingScale, 0, objCount - 2);
				else	selectingScale = 0;
				ImGui::SliderInt("Selected", &m_current, selectingScale, objCount - 1);

				if (cameraEnt != entt::null) {
					ImGui::Checkbox("Look At Selected", &m_lookingAtSelected);
					if (ImGui::Button("Select Looking At")) {
						Transform& trans = ECS::GetComponent<Transform>(cameraEnt);
						RayResult ray = PhysBody::GetRaycastResult(
							BLM::GLMtoBT(trans.GetGlobalPosition()), BLM::GLMtoBT(trans.GetForwards()));

						for (int i(0); i < m_boxShape.size(); ++i) {
							if (m_boxShape[i] == ray.m_collisionObject->getCollisionShape()) {
								m_current = i;
								break;
							}
						}
					}
				}

				if (ImGui::Button("Duplicate current as Box")) {
					btCollisionShape* tempShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
					//if (m_objects[m_current].type == "box")			tempShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
					//else if (m_objects[m_current].type == "cyl")	tempShape = new btCylinderShape(btVector3(0.5f, 0.5f, 0.5f));
					//if (tempShape == nullptr)	throw std::runtime_error("shape machine broke somehow");
					m_boxShape.push_back(tempShape);

					btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempShape);
					btRigidBody* tempBod = new btRigidBody(rbInfo);
					tempBod->getWorldTransform() = m_objects[m_current].body->getWorldTransform();
					tempBod->getCollisionShape()->setLocalScaling(
						BLM::GLMtoBT(m_objects[m_current].trans.GetScale()));

					m_world->addRigidBody(tempBod);

					m_current = m_objects.size();
					//m_objects.push_back({ m_objects[m_current].trans, tempBod, m_objects[m_current].type });
					m_objects.push_back({ m_objects[m_current].trans, tempBod, "box" });
				}

				ImGui::SameLine();

				if (ImGui::Button("Duplicate current as Cylinder")) {
					btCollisionShape* tempShape = new btCylinderShape(btVector3(0.5f, 0.5f, 0.5f));
					//if (m_objects[m_current].type == "box")			tempShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
					//else if (m_objects[m_current].type == "cyl")	tempShape = new btCylinderShape(btVector3(0.5f, 0.5f, 0.5f));
					//if (tempShape == nullptr)	throw std::runtime_error("shape machine broke somehow");
					m_boxShape.push_back(tempShape);

					btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempShape);
					btRigidBody* tempBod = new btRigidBody(rbInfo);
					tempBod->getWorldTransform() = m_objects[m_current].body->getWorldTransform();
					tempBod->getCollisionShape()->setLocalScaling(
						BLM::GLMtoBT(m_objects[m_current].trans.GetScale()));

					m_world->addRigidBody(tempBod);

					m_current = m_objects.size();
					//m_objects.push_back({ m_objects[m_current].trans, tempBod, m_objects[m_current].type });
					m_objects.push_back({ m_objects[m_current].trans, tempBod, "cyl" });

				}
			}

			//drawing circle test
			if (m_tempObjects.size() > 0) {
				if (ImGui::TreeNode("CircleTest")) {
					if (ImGui::Button("Remove Circular Test")) {
						m_tempObjects.clear();
					}
					else if (ImGui::Button("Apply Circular Test")) {
						for (int i(1); i < m_tempObjects.size(); ++i) {
							btCollisionShape* tempShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
							m_boxShape.push_back(tempShape);
							btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, tempShape);
							btRigidBody* tempBod = new btRigidBody(rbInfo);

							tempBod->getWorldTransform() = btTransform(
								BLM::GLMtoBT(m_tempObjects[i].GetLocalRotation()),
								BLM::GLMtoBT(m_tempObjects[i].GetLocalPosition())
							);
							tempBod->getCollisionShape()->setLocalScaling(BLM::GLMtoBT(m_tempObjects[i].GetScale()));
							m_objects.push_back({ m_tempObjects[i], tempBod, "box" });
						}
						m_tempObjects.clear();

					}
					else if (ImGui::TreeNode("Circular Test Data")) {
						int count = m_tempObjects.size() - 1;
						if (ImGui::SliderInt("Ring Count", &count, 1, 64)) {
							ChangeCircularCount(count);

						}

						if (ImGui::InputFloat2("Radius Bounds", bound3, 2)) {
							bound3[0] = glm::clamp(bound3[0], 0.01f, bound3[1]);
							bound3[1] = glm::clamp(bound3[1], bound3[0], 1000.f);
						}
						if (ImGui::SliderFloat("Radius", &circularRadius, bound3[0], bound3[1])) {
							ChangeCircularCount(count);
						}


						glm::vec3 value = m_tempObjects[0].GetLocalPosition();
						if (ImGui::TreeNode("Center Position")) {
							if (ImGui::InputFloat2("Position Bounds", bound, 2)) {
								bound[0] = glm::clamp(bound[0], -1000.f, bound[1]);
								bound[1] = glm::clamp(bound[1], bound[0], 1000.f);
							}
							bool changed = false;
							if (ImGui::SliderFloat("x", &value.x, bound[0], bound[1])) {
								changed = true;
							}
							if (ImGui::SliderFloat("y", &value.y, bound[0], bound[1])) {
								changed = true;
							}
							if (ImGui::SliderFloat("z", &value.z, bound[0], bound[1])) {
								changed = true;
							}
							if (changed) {
								m_objects[m_current].body->getWorldTransform().
									setOrigin(BLM::GLMtoBT(m_tempObjects[0].SetPosition(value).GetLocalPosition()));
								ChangeCircularCount(count);
							}

							ImGui::TreePop();
						}


						ImGui::SliderFloat("angle", &rotAmt, 0.f, 180.f, "%.0f");

						if (ImGui::Button(("Rotate " + std::to_string((int)rotAmt) + " on x").c_str())) {
							m_tempObjects[0].SetRotation(m_tempObjects[0].GetLocalRotation() *
								glm::angleAxis(glm::radians(rotAmt), glm::vec3(1, 0, 0))
							).GetLocalRotation();
							ChangeCircularCount(count);
						}
						ImGui::SameLine();
						if (ImGui::Button(("Rotate -" + std::to_string((int)rotAmt) + " on x").c_str())) {
							m_tempObjects[0].SetRotation(m_tempObjects[0].GetLocalRotation() *
								glm::angleAxis(glm::radians(rotAmt), glm::vec3(-1, 0, 0))
							).GetLocalRotation();
							ChangeCircularCount(count);
						}

						if (ImGui::Button(("Rotate " + std::to_string((int)rotAmt) + " on y").c_str())) {
							m_tempObjects[0].SetRotation(m_tempObjects[0].GetLocalRotation() *
								glm::angleAxis(glm::radians(rotAmt), glm::vec3(0, 1, 0))
							).GetLocalRotation();
							ChangeCircularCount(count);
						}
						ImGui::SameLine();
						if (ImGui::Button(("Rotate -" + std::to_string((int)rotAmt) + " on y").c_str())) {
							m_tempObjects[0].SetRotation(m_tempObjects[0].GetLocalRotation() *
								glm::angleAxis(glm::radians(rotAmt), glm::vec3(0, -1, 0))
							).GetLocalRotation();
							ChangeCircularCount(count);
						}

						if (ImGui::TreeNode("Piece Scale")) {
							if (ImGui::InputFloat2("Scale Bounds", bound2, 2)) {
								bound2[1] = glm::clamp(bound2[1], bound2[0], 1000.f);
								bound2[0] = glm::clamp(bound2[0], 0.01f, bound2[1]);
							}

							value = m_tempObjects[1].GetScale();
							bool changed = false;
							if (ImGui::SliderFloat("x", &value.x, bound2[0], bound2[1])) {
								value.x = glm::clamp(value.x, bound2[0], bound2[1]);
								changed = true;
							}
							if (ImGui::SliderFloat("y", &value.y, bound2[0], bound2[1])) {
								value.y = glm::clamp(value.y, bound2[0], bound2[1]);
								changed = true;
							}
							if (ImGui::SliderFloat("z", &value.z, bound2[0], bound2[1])) {
								value.z = glm::clamp(value.z, bound2[0], bound2[1]);
								changed = true;
							}

							if (changed) {
								m_tempObjects[1].SetScale(value);
								ChangeCircularCount(count);
							}
							ImGui::TreePop();
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}
			else if (ImGui::Button("Circular Test")) {
				circularRadius = 5.f;
				ChangeCircularCount(5, true);
			}
		}

		ImGui::TreePop();
	}

	if (m_objects.size() > 0 && m_draw) {
		if (m_lookingAtSelected) if (cameraEnt != entt::null)
			ECS::GetComponent<Transform>(cameraEnt).LookAt(m_objects[m_current].trans.GetGlobalPosition());

		if (ImGui::TreeNode("Transformations")) {
			ImGui::Text("Blue is Z, Green is Y, Red is X");
			auto& currentTrans = m_objects[m_current].trans;
			glm::vec3 value = currentTrans.GetLocalPosition();

			if (ImGui::TreeNode("Position")) {
				if (ImGui::InputFloat2("Position Bounds", bound, 2)) {
					bound[0] = glm::clamp(bound[0], -1000.f, bound[1]);
					bound[1] = glm::clamp(bound[1], bound[0], 1000.f);
				}

				bool changed = false;
				if (ImGui::SliderFloat("x", &value.x, bound[0], bound[1])) {
					changed = true;
				}
				if (ImGui::SliderFloat("y", &value.y, bound[0], bound[1])) {
					changed = true;
				}
				if (ImGui::SliderFloat("z", &value.z, bound[0], bound[1])) {
					changed = true;
				}
				if (changed)
					m_objects[m_current].body->getWorldTransform().
					setOrigin(BLM::GLMtoBT(currentTrans.SetPosition(value).GetLocalPosition()));

				ImGui::TreePop();
			}

			ImGui::SliderFloat("angle", &rotAmt, 0.f, 180.f, "%.0f");

			if (ImGui::Button(("Rotate " + std::to_string((int)rotAmt) + " on x").c_str())) {
				m_objects[m_current].body->getWorldTransform().setRotation(BLM::GLMtoBT(
					currentTrans.SetRotation(currentTrans.GetLocalRotation() *
						glm::angleAxis(glm::radians(rotAmt), glm::vec3(1, 0, 0))
					).GetLocalRotation())
				);
			}
			ImGui::SameLine();
			if (ImGui::Button(("Rotate -" + std::to_string((int)rotAmt) + " on x").c_str())) {
				m_objects[m_current].body->getWorldTransform().setRotation(BLM::GLMtoBT(
					currentTrans.SetRotation(currentTrans.GetLocalRotation() *
						glm::angleAxis(glm::radians(rotAmt), glm::vec3(-1, 0, 0))
					).GetLocalRotation())
				);
			}

			if (ImGui::Button(("Rotate " + std::to_string((int)rotAmt) + " on y").c_str())) {
				m_objects[m_current].body->getWorldTransform().setRotation(BLM::GLMtoBT(
					currentTrans.SetRotation(currentTrans.GetLocalRotation() *
						glm::angleAxis(glm::radians(rotAmt), glm::vec3(0, 1, 0))
					).GetLocalRotation())
				);
			}
			ImGui::SameLine();
			if (ImGui::Button(("Rotate -" + std::to_string((int)rotAmt) + " on y").c_str())) {
				m_objects[m_current].body->getWorldTransform().setRotation(BLM::GLMtoBT(
					currentTrans.SetRotation(currentTrans.GetLocalRotation() *
						glm::angleAxis(glm::radians(rotAmt), glm::vec3(0, -1, 0))
					).GetLocalRotation())
				);
			}

			if (ImGui::TreeNode("Scale")) {
				if (ImGui::InputFloat2("Scale Bounds", bound2, 2)) {
					bound2[1] = glm::clamp(bound2[1], bound2[0], 1000.f);
					bound2[0] = glm::clamp(bound2[0], 0.01f, bound2[1]);
				}

				value = currentTrans.GetScale();
				bool changed = false;
				if (ImGui::SliderFloat("x", &value.x, bound2[0], bound2[1])) {
					value.x = glm::clamp(value.x, bound2[0], bound2[1]);
					changed = true;
				}
				if (ImGui::SliderFloat("y", &value.y, bound2[0], bound2[1])) {
					value.y = glm::clamp(value.y, bound2[0], bound2[1]);
					changed = true;
				}
				if (m_objects[m_current].type == "cyl") {
					//cylinders can't have different z and x values
					value.z = value.x;
				}
				else if (ImGui::SliderFloat("z", &value.z, bound2[0], bound2[1])) {
					value.z = glm::clamp(value.z, bound2[0], bound2[1]);
					changed = true;
				}

				if (changed) {
					currentTrans.SetScale(value);
					m_objects[m_current].body->getCollisionShape()->
						setLocalScaling(BLM::GLMtoBT(value));
				}
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}
}

void HitboxGen::ChangeCircularCount(int amt, bool init)
{
	if (amt < 1)	return;

	if (init)
		m_tempObjects.push_back(Transform());

	int count = m_tempObjects.size() - 1;

	//fix count
	if (amt < count) {
		//smaller
		while (amt < count) {
			m_tempObjects.pop_back();
			count = m_tempObjects.size() - 1;
		}
	}
	else if (amt > count) {
		//larger
		while (amt > count) {
			m_tempObjects.push_back(Transform());
			count = m_tempObjects.size() - 1;
		}
	}

	count = m_tempObjects.size();

	//do all the fixes
	glm::mat4 offsetModel = m_tempObjects[0].GetModel();
	Transform base;
	if (!init)	base = m_tempObjects[1];

	float angleOffset = glm::two_pi<float>() / amt;
	glm::vec3 distanceFromCenter = glm::vec3(0, 0, circularRadius);
	glm::quat rotation = glm::angleAxis(0.f, BLM::GLMup);

	for (int i(1); i < count; ++i) {
		//store the base transform (really just the scale)
		m_tempObjects[i] = base;
		m_tempObjects[i].SetPosition(glm::rotate(rotation, distanceFromCenter));
		m_tempObjects[i].SetRotation(rotation);
		m_tempObjects[i].UseAsParent(offsetModel);

		rotation = glm::angleAxis(angleOffset * i, BLM::GLMup);
	}

}
