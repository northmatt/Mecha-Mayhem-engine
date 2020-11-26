#pragma once
#include "Components/ObjMorphLoader.h"
#include "Components/Transform.h"

class HitboxGen
{
public:
	HitboxGen() {}
	~HitboxGen() { m_world = nullptr; }

	void ToggleDraw() {
		if (m_draw = !m_draw)		std::cout << "drawing hitboxes    \r";
		else						std::cout << "not drawing hitboxes\r";
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

	//reads inputs:
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
	void Update(float dt);

	//init things
	static void Init() {
		m_cube.LoadMesh("models/GodHimself.obj");
		m_cubeCurrent.LoadMesh("models/GodHimself.obj", true);
		m_planeShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	}

private:
	static ObjLoader m_cube;
	static ObjLoader m_cubeCurrent;
	static btCollisionShape* m_planeShape;
	static Transform m_defaultTrans;

	bool m_draw = false;
	size_t m_current = 0;
	std::string m_filename;

	btDiscreteDynamicsWorld *m_world = nullptr;

	struct Box
	{
		Transform trans;
		btRigidBody* body;
	};

	std::vector<Box> m_objects = {};
	btRigidBody* m_floor = nullptr;
	btAlignedObjectArray<btCollisionShape*> m_boxShape;
};

