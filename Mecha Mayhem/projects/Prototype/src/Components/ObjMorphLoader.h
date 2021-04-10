#pragma once
#include "MultiTextObj.h"

class ObjMorphLoader
{
public:
	//contructor
	ObjMorphLoader() {}
	ObjMorphLoader(const std::string& baseFileName, bool usingMaterial = false);
	~ObjMorphLoader();

	ObjMorphLoader& LoadMeshs(const std::string& baseFileName, bool usingMaterial = false);

	//will not load if file doesn't exist
	void BlendTo(const std::string& baseFileName, float delay = 0.1f, int frame = 0);

	static void Init();
	static void Unload();
	
	ObjMorphLoader& SetDirection(bool forwards) {
		if (m_reversing == forwards)
			ToggleDirection();
		return *this;
	}

	bool IsDone() { return (m_t == 1) && !m_loop; }
	size_t Getp0() { return m_p0; }
	size_t Getp1() { return m_p1; }

	bool IsAnim(const std::string& test) { return m_anims[m_index].fileName == test; }

	//returns true if moving forwards
	bool GetDirection() { return !m_reversing; }

	ObjMorphLoader& ToggleDirection();

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

	ObjMorphLoader& SetReceiveShadows(bool choice) { m_receiveShadows = choice; return *this; }
	bool GetReceiveShadows() { return m_receiveShadows; }
	ObjMorphLoader& SetCastShadows(bool choice) { m_castShadows = choice; return *this; }
	bool GetCastShadows() { return m_castShadows; }
	ObjMorphLoader& SetRimLighting(bool choice) { m_rimLighting = choice; return *this; }
	bool GetRimLighting() { return m_rimLighting; }

	void Update(float dt);

	static void BeginDraw(unsigned amt = 0, unsigned transAmt = 0);
	static void BeginTempDraw();

	void DrawTrans(const glm::mat4& model, const glm::vec3& colour = BLM::GLMzero);
	void Draw(const glm::mat4& model, const glm::vec3& colour = BLM::GLMzero);
	void DrawTemp(const glm::mat4& model, const glm::vec3& colour = BLM::GLMzero);

	/*static void PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour,
		const std::array<glm::vec3, MAX_LIGHTS>& lightPos, const std::array<glm::vec3, MAX_LIGHTS>& lightColour, const int& lightCount,
		float specularStrength = 1.f, float shininess = 4,
		float ambientLightStrength = 0.05f, const glm::vec3& ambientColour = glm::vec3(0.f), float ambientStrength = 0.f
	);*/

	static void PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour, float emissiveness = 0, float shininess = 4);
	static void PerformDrawTrans(const glm::mat4& view, const Camera& camera);

	static void PerformDrawShadow(const glm::mat4& lightVPMatrix);

	glm::vec3 GetAdditiveColour() { return m_colour; }
	void SetAdditiveColour(const glm::vec3& colour) { m_colour = colour; }

	void Enable() { m_enabled = true; }
	void Disable() { m_enabled = false; }
	bool GetEnabled() { return m_enabled; }

	//deletes teh vao;
	void Destroy() { m_vao = nullptr; }

private:
	struct Frames {
		VertexBuffer::sptr pos;
		VertexBuffer::sptr normal;
		VertexBuffer::sptr colour = nullptr;
		VertexBuffer::sptr spec = nullptr;
	};

	struct Animations {
		std::string fileName;
		bool mat;
		bool text = false;
		bool bounce = false;
		bool loop = false;
		bool reversing = false;
		float startTime = 0;
		std::vector<float> durations = {};
		std::vector<size_t> frameIndices = {};
		std::vector<Frames> frames = {};
		size_t start = 0;
		size_t t = 0;
		int texture = -1;
		VertexBuffer::sptr UVs = nullptr;
	};

	struct DrawData {
		float t;
		VertexArrayObject::sptr vao;
		glm::mat4 model;
		glm::vec3 colour;
		int shaded;
		int rimLit;
		int texture = -1;
	};

	static std::vector<DrawData> m_transQueue;
	static std::vector<DrawData> m_texQueue;
	static std::vector<DrawData> m_matQueue;
	static std::vector<DrawData> m_defaultQueue;
	static std::vector<DrawData> m_texTempQueue;
	static std::vector<DrawData> m_matTempQueue;
	static std::vector<DrawData> m_defaultTempQueue;
	static std::vector<Animations> m_anims;
	static Shader::sptr m_shader;
	static Shader::sptr m_matShader;
	static Shader::sptr m_texShader;
	static Shader::sptr m_shadowShader;

	static const std::vector<BufferAttribute> pos1Buff;
	static const std::vector<BufferAttribute> pos2Buff;
	static const std::vector<BufferAttribute> norm1Buff;
	static const std::vector<BufferAttribute> norm2Buff;
	static const std::vector<BufferAttribute> col1Buff;
	static const std::vector<BufferAttribute> col2Buff;
	static const std::vector<BufferAttribute> spec1Buff;
	static const std::vector<BufferAttribute> spec2Buff;
	static const std::vector<BufferAttribute> UVBuff;

	VertexArrayObject::sptr m_vao = nullptr;

	float m_timer = 0;
	float m_t = 0;
	float m_speed = 1;
	float m_transitionSpeed = 0;

	size_t m_p0 = 0;
	size_t m_p1 = 0;
	size_t m_p0Hold = 0;
	int m_index = -1;
	int m_indexHold = -1;

	bool m_castShadows = true;
	bool m_receiveShadows = true;
	bool m_rimLighting = false;
	bool m_reversing = false;
	bool m_bounce = false;
	bool m_loop = false;
	bool m_blend = false;
	bool m_enabled = false;

	glm::vec3 m_colour = BLM::GLMzero;
};