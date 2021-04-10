#pragma once
#include <Logging.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>

#include <GLM/gtc/type_ptr.hpp>

#include "Engine/BackEnd.h"
#include "Engine/Texture2D.h"
#include "Camera.h"
#include "Utilities/BLM.h"

class Sprite
{
public:
	Sprite() {}
	
	//assumes in texture folder
	Sprite(const std::string& textureName, float width, float height);
	Sprite(const glm::vec4& colour, float width, float height);

	static void Init();

	static void Unload();

	//assumes in texture folder
	Sprite& Init(const std::string& textureName, float width, float height);

	//generates the colour, name is the RGB value
	Sprite& Init(const glm::vec4& colour, float width, float height);

	static void BeginDraw(unsigned amt = 0);
	static void BeginUIDraw(unsigned UIamt = 0, unsigned camCount = 0);

	void Draw(const glm::mat4& VP, const glm::mat4& model, const glm::vec3& addColour = BLM::GLMzero);
	void DrawSingle(const glm::mat4& VP, const glm::mat4& model, const glm::vec3& addColour = BLM::GLMzero);
	void DrawToUI(const glm::mat4& VP, const glm::mat4& model, short camNum, const glm::vec3& addColour = BLM::GLMzero);

	static void PerformDraw();
	static void PerformUIDraw(int numOfCams);
	
	static void PerformDrawShadow(const glm::mat4& lightVPMatrix);

	void SetScale(float scl) { m_scale = scl; }
	void SetWidth(float width) { m_width = width; }
	void SetHeight(float height) { m_height = height; }

	bool IsValid() { return (m_width != 0) && (m_height != 0) && (m_scale != 0); }

	//if ever you just want the texture
	void Bind(int slot) {
		m_textures[m_index].texture->Bind(slot);
	}

	//basically just Texture2D::Unbind();
	void UnBind(int slot) {
		Texture2D::Unbind(slot);
	}

	struct Texture
	{
		std::string fileName;
		Texture2D::sptr texture;
	};

	static std::vector<Texture> m_textures;

	Sprite& SetReceiveShadows(bool choice) { m_receiveShadows = choice; return *this; }
	bool GetReceiveShadows() { return m_receiveShadows; }

	Sprite& SetCastShadows(bool choice) { m_castShadows = choice; return *this; }
	bool GetCastShadows() { return m_castShadows; }

	Sprite& SetEnabled(bool choice) { m_enabled = choice;	return *this; }
	bool GetEnabled() { return m_enabled; }

private:
	struct DrawData {
		size_t index;
		glm::mat4 model;
		glm::mat4 MVP;
		glm::vec3 additiveColour;
		int receiveShadows;
	};
	static std::vector<DrawData> m_Queue;
	static std::vector<DrawData> m_UIQueue[4];
	static VertexArrayObject::sptr m_square;
	static Shader::sptr m_shader;

	bool m_enabled = true;
	bool m_castShadows = true;
	bool m_receiveShadows = true;

	size_t m_index = 0;
	float m_width = 0;
	float m_height = 0;
	float m_scale = 1;
};

