#pragma once
#include "Components/Transform.h"
#include "Components/ObjMorphLoader.h"
#include "Components/SpawnStation.h"

class HitboxGen
{
public:
	HitboxGen() {}
	~HitboxGen() { m_world = nullptr; }

	bool ToggleDraw() {
		return m_draw = !m_draw;
	}

	bool ToggleDrawSpawns() {
		return m_drawSpawns = !m_drawSpawns;
	}

	//gets the physicsworld and reads/create the file.
	//returns false if failed, also stores the current world
	bool Init(btDiscreteDynamicsWorld* world, const std::string& filename, bool newFile = true, bool overwrite = true);

	//adds extension, use for loading maps from other scenes
	void SetFileName(const std::string& filename) { m_filename = "maps/" + filename; }

	//saves all cube locations to the file
	//returns false if overwrite is false and the file exists
	bool SaveToFile(bool overwriteExisting = true);

	//returns false if failed
	bool LoadFromFile();

	//all cubes are sent to OBJ draw queue if draw is true
	void Render();

	//reads inputs (depreciated):
	//IJKL,. to move current (based on rotation),
	// -LCTRL to speed up these inputs
	//789 to scale (xyz) current (rgb coloured),
	// -LCTRL to invert change
	//TFGH to rotate current (increments in 45ds),
	//YU to move back and forth in the vector,
	//N to create new cube at (0,0,0),
	//M to duplicate current
	//B to delete current
	//[] to move the floor up and down
	// -LCTRL to slow down
	//TAB to slow everything down

	//Through ImGui
	void Update(entt::entity cameraEnt = entt::null);

	//only call once please
	void GenerateSpawners() {
		for (int i(0); i < m_spawners.size(); ++i) {
			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Spawner>(entity).Init(m_spawners[i].radius, m_spawners[i].delay)
				.SetBounds(m_spawners[i].bounds[0], m_spawners[i].bounds[1]);
			ECS::GetComponent<Transform>(entity).SetPosition(m_spawners[i].pos).SetRotation(m_spawners[i].rot);
		}
	}

	//returns position
	glm::vec3 SetSpawnNear(Player& p, glm::vec3 testPos, float range) {
		if (m_spawnLocations.size() == 0)	return BLM::GLMzero;

		//for loop in case it never hits anything
		int index = rand() % m_spawnLocations.size();
		glm::vec3 pos = m_spawnLocations[index].pos;
		//limit how many tries it does (in case all attempts fail)
		for (int i(0); i < 25; ++i) {
			if (glm::length(testPos - pos) < range) {
				//success
				break;
			}
			//re-get one
			index = rand() % m_spawnLocations.size();
			pos = m_spawnLocations[index].pos;
		}

		p.SetSpawn(pos);
		p.SetRotation(m_spawnLocations[index].roty, m_spawnLocations[index].rotx);

		return pos;
	}

	void SetSpawnAvoid(Player& p, std::vector<glm::vec3> tests, float range) {
		if (m_spawnLocations.size() == 0)	return;

		//test for players or limits or whatever
		int index = rand() % m_spawnLocations.size();
		glm::vec3 pos = m_spawnLocations[index].pos;
		//limit how many tries it does (in case all attempts fail)
		for (int i(0); i < 25; ++i) {
			bool works = true;
			for (int x(0); x < tests.size(); ++x) {
				//if too close, try again
				if (glm::length(tests[x] - pos) < range) {
					works = false;
					break;
				}
			}
			if (works)
				break;

			//re-get one
			index = rand() % m_spawnLocations.size();
			pos = m_spawnLocations[index].pos;
		}

		p.SetSpawn(pos);
		p.SetRotation(m_spawnLocations[index].roty, m_spawnLocations[index].rotx);
	}

	void Clear() {
		m_draw = false;
		m_current = 0;
		m_world = nullptr;
		m_objects.clear();
		for (int i(0); i < m_boxShape.size(); ++i) {
			delete m_boxShape[i];
		}
		m_boxShape.clear();
		m_spawners.clear();
		m_spawnLocations.clear();
	}

	//init things
	static void Init() {
		m_cube.LoadMesh("maps/GodHimself.obj");
		m_cylinder.LoadMesh("maps/Pillar.obj");
		m_cubeCurrent.LoadMesh("maps/GodHimself.obj", true);
		m_cylinderCurrent.LoadMesh("maps/Pillar.obj", true);
		m_planeShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	}

private:
	void ChangeCircularCount(int amt, bool init = false);

	static ObjLoader m_cube;
	static ObjLoader m_cylinder;
	static ObjLoader m_cubeCurrent;
	static ObjLoader m_cylinderCurrent;
	static btCollisionShape* m_planeShape;
	static Transform m_defaultTrans;

	static const bool printText = false;

	float m_speedModifier = 1.f;
	float rotAmt = 45.f;
	float circularRadius = 5.f;
	float floorHeight[2] = { -40.f, 40.f };
	float bound[2] = { -100.f, 100.f };
	float bound2[2] = { 1.f, 100.f };
	float bound3[2] = { 0.01f, 100.f };
	bool m_draw = false;
	bool m_drawSpawns = false;
	bool m_lookingAtSelected = false;
	int m_current = 0;
	int m_spawnerCurrent = 0;
	int m_spawnLocCurrent = 0;
	int selectingScale = 0;
	std::string m_filename;

	btDiscreteDynamicsWorld *m_world = nullptr;

	struct Shape
	{
		Transform trans;
		btRigidBody* body;
		std::string type = "box";
	};
	std::vector<Shape> m_objects = {};

	const glm::vec2 __spawnerBounds = { 0, 6 };	//check player.h for more info

	struct SpawnerData
	{
		glm::vec3 pos;
		glm::quat rot;
		glm::vec2 bounds;
		float radius;
		float delay;
	};
	std::vector<SpawnerData> m_spawners = {};

	struct SpawnData
	{
		glm::vec3 pos;
		float rotx;
		float roty;
	};
	std::vector<SpawnData> m_spawnLocations = {};

	std::vector<Transform> m_tempObjects = {};
	btRigidBody* m_floor = nullptr;
	btAlignedObjectArray<btCollisionShape*> m_boxShape;
};

