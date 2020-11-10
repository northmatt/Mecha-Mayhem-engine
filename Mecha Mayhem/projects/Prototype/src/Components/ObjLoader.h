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

#include "Engine/BackEnd.h"
#include "Components/Camera.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DData.h"

struct Models
{
	std::string fileName;
	bool mat;
	bool text = false;
	size_t texture = 0;
	//std::vector<VertexArrayObject::sptr> vao = {};
	//std::vector<size_t> verts = {};
	VertexArrayObject::sptr vao = VertexArrayObject::Create();
	size_t verts = 0;
};

struct Texture
{
	std::string fileName;
	Texture2D::sptr texture;
};

struct DrawData
{
	unsigned entityIndex;
	size_t modelIndex;
	glm::mat4 model;
};

class ObjLoader
{
public:
	//contructor
	ObjLoader() {}
	ObjLoader(const std::string& fileName, bool usingMaterial = false);
	~ObjLoader();

	ObjLoader& LoadMesh(const std::string& fileName, bool usingMaterial = false);

	static void Init();
	static void Unload();

	static void BeginDraw();

	void Draw(unsigned entity, glm::mat4 model);

	static void PerformDraw(glm::mat4 view, Camera camera, glm::vec3 colour, glm::vec3 lightPos, glm::vec3 lightColour = glm::vec3(1.f),
		float specularStrength = 1.f, float shininess = 4,
		float ambientLightStrength = 0.05f, glm::vec3 ambientColour = glm::vec3(0.f), float ambientStrength = 0.1f
	);

	Shader::sptr GetShader() { return m_shader; }

private:
	static std::vector<DrawData> m_matQueue;
	static std::vector<DrawData> m_texQueue;
	static std::vector<DrawData> m_defaultQueue;
	static Shader::sptr m_shader;
	static Shader::sptr m_matShader;
	static Shader::sptr m_texShader;
	static std::vector<Models> m_models;
	static std::vector<Texture> m_textures;

	size_t m_index = INT_MAX;
	size_t m_texture = INT_MAX;
	unsigned m_drawID = INT_MAX;
	bool m_usingMaterial = false;
	bool m_usingTexture = false;
	bool m_addedToDraw = false;
};