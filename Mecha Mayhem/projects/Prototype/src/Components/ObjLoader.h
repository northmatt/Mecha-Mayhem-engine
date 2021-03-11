#pragma once
#include "Sprite.h"

class ObjLoader
{
	friend class MultiTextObj;
	friend class Sprite;
public:
	//contructor
	ObjLoader() {}
	ObjLoader(const std::string& fileName, bool usingMaterial = false);
	~ObjLoader();

	ObjLoader& LoadMesh(const std::string& fileName, bool usingMaterial = false);

	static void Init();
	static void Unload();

	static void BeginDraw(unsigned amt = 0);

	static void BeginTempDraw();

	void Draw(const glm::mat4& model, const glm::vec3& colour = glm::vec3(0.f));
	void DrawTemp(const glm::mat4& model, const glm::vec3& colour = glm::vec3(0.f));

	static void PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour, const std::array<glm::vec3, MAX_LIGHTS>& lightPos, const std::array<glm::vec3, MAX_LIGHTS>& lightColour, const int& lightCount,
		float specularStrength = 1.f, float shininess = 4,
		float ambientLightStrength = 0.05f, const glm::vec3& ambientColour = glm::vec3(0.f), float ambientStrength = 0.f
	);
	static void PerformDrawShadow(const glm::mat4& lightVPMatrix);

	void Enable() { m_enabled = true; }
	void Disable() { m_enabled = false; }
	bool GetEnabled() { return m_enabled; }

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
		glm::vec3 colour;
	};

	static std::vector<DrawData> m_matQueue;
	static std::vector<DrawData> m_texQueue;
	static std::vector<DrawData> m_defaultQueue;
	static std::vector<DrawData> m_matTempQueue;
	static std::vector<DrawData> m_texTempQueue;
	static std::vector<DrawData> m_defaultTempQueue;
	static std::vector<Models> m_models;
	static Shader::sptr m_shader;
	static Shader::sptr m_matShader;
	static Shader::sptr m_texShader;
	static Shader::sptr m_shadowShader;

	static std::vector<BufferAttribute> m_posAttrib;
	static std::vector<BufferAttribute> m_normAttrib;
	static std::vector<BufferAttribute> m_colAttrib;
	static std::vector<BufferAttribute> m_specAttrib;
	static std::vector<BufferAttribute> m_uvAttrib;

	bool m_enabled = true;
	size_t m_index = INT_MAX;
};