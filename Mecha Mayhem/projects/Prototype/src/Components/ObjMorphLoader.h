#pragma once
#include "ObjLoader.h"

class ObjMorphLoader
{
public:
	//contructor
	ObjMorphLoader() {}
	ObjMorphLoader(const std::string& baseFileName, bool usingMaterial = false);
	~ObjMorphLoader();

	ObjMorphLoader& LoadMeshs(const std::string& baseFileName, bool usingMaterial = false);

	static void Init();
	static void Unload();
	
	ObjMorphLoader& SetDirection(bool forwards) {
		if (m_reversing == forwards)
			ToggleDirection();
		return *this;
	}

	//returns true if moving forwards
	bool GetDirection() { return !m_reversing; }

	ObjMorphLoader& ToggleDirection() {
		auto& data = m_anims[m_index];
		if (m_reversing = !m_reversing) {
			m_p1 = ((m_p0 = m_p1) == 0 ? m_anims[m_index].durations.size() - 1 : m_p1 - 1);
			m_timer = m_anims[m_index].durations[m_p0] - m_timer;
		}
		else {
			m_p0 = ((m_p1 = m_p0) == 0 ? m_anims[m_index].durations.size() - 1 : m_p0 - 1);
			m_timer = m_anims[m_index].durations[m_p1] - m_timer;
		}
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
		if (m_usingMaterial) {
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
		}
		return *this;
	}

	ObjMorphLoader& SetBounce(bool bounce) {
		m_bounce = bounce;
		return *this;
	}

	bool GetBounce() { return m_bounce; }

	ObjMorphLoader& ToggleBounce() {
		m_bounce = !m_bounce;
		return *this;
	}

	ObjMorphLoader& SetLooping(bool loop) {
		m_loop = loop;
		return *this;
	}

	bool GetLooping() { return m_loop; }

	ObjMorphLoader& ToggleLooping() {
		m_loop = !m_loop;
		return *this;
	}

	ObjMorphLoader& SetSpeed(float speed) { m_speed = speed; return *this; }
	float GetSpeed() { return m_speed; }

	static void BeginDraw();

	void Update(float dt);

	void Draw(const glm::mat4& model);

	static void PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour,
		const glm::vec3& lightPos, const glm::vec3& lightColour = glm::vec3(1.f),
		float specularStrength = 1.f, float shininess = 4,
		float ambientLightStrength = 0.05f, const glm::vec3& ambientColour = glm::vec3(0.f), float ambientStrength = 0.f
	);

private:
	struct Frames {
		VertexBuffer::sptr pos;
		VertexBuffer::sptr normal;
		VertexBuffer::sptr colour = nullptr;
	};

	struct Animations {
		std::string fileName;
		bool mat;
		std::vector<float> durations;
		std::vector<size_t> frameIndices;
		std::vector<Frames> frames = {};
		VertexBuffer::sptr spec = nullptr;
	};

	struct DrawData {
		float t;
		VertexArrayObject::sptr vao;
		glm::mat4 model;
	};

	static std::vector<DrawData> m_matQueue;
	static std::vector<DrawData> m_defaultQueue;
	static std::vector<Animations> m_anims;
	static Shader::sptr m_shader;
	static Shader::sptr m_matShader;
	static std::vector<BufferAttribute> pos1Buff;
	static std::vector<BufferAttribute> pos2Buff;
	static std::vector<BufferAttribute> norm1Buff;
	static std::vector<BufferAttribute> norm2Buff;
	static std::vector<BufferAttribute> col1Buff;
	static std::vector<BufferAttribute> col2Buff;
	static std::vector<BufferAttribute> specBuff;

	VertexArrayObject::sptr m_vao = nullptr;

	float m_timer = 0;
	float m_t = 0;
	float m_speed = 1;

	size_t m_p0 = 0;
	size_t m_p1 = 0;
	size_t m_index = INT_MAX;
	bool m_usingMaterial = false;

	bool m_reversing = false;
	bool m_bounce = false;
	bool m_loop = false;
};