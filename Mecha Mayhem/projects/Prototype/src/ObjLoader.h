#pragma once
#include <Logging.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>

#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "BackEndThings/BackEnd.h"
#include "Camera.h"

struct Models
{
	std::string fileName;
	bool mat;
	std::vector<VertexArrayObject::sptr> vao = {};
	std::vector<size_t> verts = {};
};

class ObjLoader
{
public:
	//contructor
	ObjLoader() {}
	ObjLoader(const std::string& fileName, bool usingMaterial = false);
	~ObjLoader();

	static void Init();

	void LoadMesh(const std::string& fileName, bool usingMaterial = false);

	void Draw(Camera camera, glm::mat4 transform, glm::mat3 rotation, glm::vec3 colour,
		glm::vec3 lightPos, glm::vec3 lightColour = glm::vec3(1.f), float specularStrength = 1.f, float shininess = 4,
		float ambientLightStrength = 0.05f, glm::vec3 ambientColour = glm::vec3(0.f), float ambientStrength = 0.1f
	);

	Shader::sptr GetShader() { return m_shader; }

private:
	static Shader::sptr m_shader;
	static Shader::sptr m_matShader;
	static std::vector<Models> m_models;

	size_t m_index = 0;
	bool m_usingMaterial = false;
};