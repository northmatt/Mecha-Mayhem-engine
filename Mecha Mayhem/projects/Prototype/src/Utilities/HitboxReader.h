#pragma once
#include "Components/ObjMorphLoader.h"
#include "Components/Transform.h"

class HitboxGen
{
public:
	HitboxGen() {}
	~HitboxGen() { m_world = nullptr; }

	bool ToggleDraw() {
		if (m_draw = !m_draw)		std::cout << "drawing hitboxes    \r";
		else						std::cout << "not drawing hitboxes\r";

		return m_draw;
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

	void Clear() {
		m_draw = false;
		m_current = 0;
		m_world = nullptr;
		m_objects.clear();
		for (int i(0); i < m_boxShape.size(); ++i) {
			delete m_boxShape[i];
		}
		m_boxShape.clear();
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

	float m_speedModifier = 1.f;
	float rotAmt = 45.f;
	float circularRadius = 5.f;
	float floorHeight[2] = { -40.f, 40.f };
	float bound[2] = { -100.f, 100.f };
	float bound2[2] = { 1.f, 100.f };
	float bound3[2] = { 0.01f, 100.f };
	bool m_draw = false;
	bool m_lookingAtSelected = false;
	int m_current = 0;
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
	std::vector<Transform> m_tempObjects = {};
	btRigidBody* m_floor = nullptr;
	btAlignedObjectArray<btCollisionShape*> m_boxShape;
};

