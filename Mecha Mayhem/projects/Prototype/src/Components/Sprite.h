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
#include "Engine/Texture2D.h"
#include "Camera.h"

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

	void Draw(const glm::mat4& VP, const glm::mat4& model);
	void DrawSingle(const glm::mat4& VP, const glm::mat4& model);
	void DrawToUI(const glm::mat4& VP, const glm::mat4& model, short camNum);

	static void PerformDraw();
	static void PerformUIDraw(int numOfCams);
	
	//lightVPMatrix sent in ObjLoader
	static void PerformDrawShadow(/*const glm::mat4& lightVPMatrix*/);

	void SetScale(float scl) { m_scale = scl; }
	void SetWidth(float width) { m_width = width; }
	void SetHeight(float height) { m_height = height; }

	bool IsValid() { return (m_width > 0) && (m_height > 0); }

	struct Texture
	{
		std::string fileName;
		Texture2D::sptr texture;
	};

	static std::vector<Texture> m_textures;

private:
	struct DrawData {
		size_t index;
		glm::mat4 MVP;
	};
	static std::vector<DrawData> m_Queue;
	static std::vector<DrawData> m_UIQueue[4];
	static VertexArrayObject::sptr m_square;
	static Shader::sptr m_shader;

	size_t m_index = 0;
	float m_width = 0;
	float m_height = 0;
	float m_scale = 1;
};

