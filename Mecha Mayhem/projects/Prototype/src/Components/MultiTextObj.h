#pragma once
#include "ObjLoader.h"

class MultiTextObj
{
public:
	//contructor
	MultiTextObj() {}
	MultiTextObj(const std::string& fileName);
	~MultiTextObj();

	static void Unload();

	MultiTextObj& LoadMesh(const std::string& fileName);

	void Draw(const glm::mat4& model, const glm::mat4& view, const Camera& camera, const glm::vec3& colour, const std::array<glm::vec3, MAX_LIGHTS>& lightPos, const std::array<glm::vec3, MAX_LIGHTS>& lightColour, const int& lightCount,
		float specularStrength = 1.f, float shininess = 4,
		float ambientLightStrength = 0.05f, const glm::vec3& ambientColour = glm::vec3(0.f), float ambientStrength = 0.f
	);
	void DrawShadow(const glm::mat4& model);

	void Enable() { m_enabled = true; }
	void Disable() { m_enabled = false; }
	bool GetEnabled() { return m_enabled; }

private:

	struct Models
	{
		std::string fileName;
		//multiple texture indices
		std::vector<size_t> texture = {};
		//multiple pieces with different mats
		std::vector<VertexArrayObject::sptr> vao = {};
	};

	static std::vector<Models> m_models;

	bool m_enabled = true;
	size_t m_index = INT_MAX;
};