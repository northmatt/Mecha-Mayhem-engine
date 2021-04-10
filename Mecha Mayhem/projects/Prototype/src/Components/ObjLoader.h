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

	void Draw(const glm::mat4& model, const glm::vec3& colour = BLM::GLMzero);
	void DrawTemp(const glm::mat4& model, const glm::vec3& colour = BLM::GLMzero);

	/*static void PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour, const std::array<glm::vec3, MAX_LIGHTS>& lightPos, const std::array<glm::vec3, MAX_LIGHTS>& lightColour, const int& lightCount,
		float specularStrength = 1.f, float shininess = 4,
		float ambientLightStrength = 0.05f, const glm::vec3& ambientColour = glm::vec3(0.f), float ambientStrength = 0.f
	);*/
	static void PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour, float emissiveness = 0, float shininess = 4);

	static void PerformDrawShadow(const glm::mat4& lightVPMatrix);

	void Enable() { m_enabled = true; }
	void Disable() { m_enabled = false; }
	bool GetEnabled() { return m_enabled; }

	glm::vec3 GetAdditiveColour() { return m_colour; }
	void SetAdditiveColour(const glm::vec3& colour) { m_colour = colour; }

	ObjLoader& SetReceiveShadows(bool choice) { m_receiveShadows = choice; return *this; }
	bool GetReceiveShadows() { return m_receiveShadows; }
	ObjLoader& SetCastShadows(bool choice) { m_castShadows = choice; return *this; }
	bool GetCastShadows() { return m_castShadows; }
	ObjLoader& SetRimLighting(bool choice) { m_rimLighting = choice; return *this; }
	bool GetRimLighting() { return m_rimLighting; }
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
		int modelIndex;
		glm::mat4 model;
		glm::vec3 colour;
		int shaded;
		int rimLit;
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

	static const std::vector<BufferAttribute> m_posAttrib;
	static const std::vector<BufferAttribute> m_normAttrib;
	static const std::vector<BufferAttribute> m_colAttrib;
	static const std::vector<BufferAttribute> m_specAttrib;
	static const std::vector<BufferAttribute> m_uvAttrib;

	bool m_castShadows = true;
	bool m_receiveShadows = true;
	bool m_rimLighting = false;
	bool m_enabled = false;
	int m_index = -1;
	glm::vec3 m_colour = BLM::GLMzero;
};