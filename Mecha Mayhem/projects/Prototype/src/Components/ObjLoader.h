#pragma once
#include "Sprite.h"
#include "Effects/Post/ColourCorrectionEffect.h"

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

	static void BeginDraw(unsigned amt = 0);

	void Draw(const glm::mat4& model);

	static void PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour, const std::array<glm::vec3, MAX_LIGHTS>& lightPos, const std::array<glm::vec3, MAX_LIGHTS>& lightColour, const int& lightCount,
		float specularStrength = 1.f, float shininess = 4,
		float ambientLightStrength = 0.05f, const glm::vec3& ambientColour = glm::vec3(0.f), float ambientStrength = 0.f
	);

	void Enable() { m_enabled = true; }
	void Disable() { m_enabled = false; }
	bool GetEnabled() { return m_enabled; }

	static bool usingAmbient;
	static bool usingSpecular;
	static bool usingDiffuse;
	static bool noLighting;
	static bool toonShading;
	static bool usingDiffuseRamp;
	static bool usingSpecularRamp;

	static Texture2D::sptr diffuseRamp;
	static Texture2D::sptr specularRamp;

	static LUT3D warm;
	static LUT3D cold;
	static LUT3D custom;

private:

	struct Models
	{
		std::string fileName;
		bool mat;
		bool text = false;
		size_t texture = INT_MAX;
		VertexArrayObject::sptr vao = VertexArrayObject::Create();
	};

	struct DrawData
	{
		size_t modelIndex;
		glm::mat4 model;
	};

	static std::vector<DrawData> m_matQueue;
	static std::vector<DrawData> m_texQueue;
	static std::vector<DrawData> m_defaultQueue;
	static std::vector<Models> m_models;
	static Shader::sptr m_shader;
	static Shader::sptr m_matShader;
	static Shader::sptr m_texShader;

	bool m_enabled = true;
	size_t m_index = INT_MAX;
};