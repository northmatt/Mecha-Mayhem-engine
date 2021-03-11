#pragma once
#include "Components/ObjMorphLoader.h"
#include "Time.h"

class Effects
{
public:
	Effects() {}

	static void Init() {
		laser = ObjMorphLoader("effects/laser", true);
		dash = ObjMorphLoader("effects/dash", true);
	}

	static void Unload() {
		laser = {};
		dash = {};
	}

	void ShootLaser(const glm::quat& rotation, const glm::vec3& position, float length, const glm::vec3& colour = BLM::GLMzero) {
		glm::mat4 model = glm::translate(BLM::GLMMat, position);
		model = model * glm::toMat4(rotation);
		model = glm::scale(model, glm::vec3(1, 1, length));

		m_effects.push_back({ laser, model, colour });
	}

	void ShootDash(const glm::quat& rotation, const glm::vec3& position, float length) {
		glm::mat4 model = glm::translate(BLM::GLMMat, position);
		model = model * glm::toMat4(rotation);
		model = glm::scale(model, glm::vec3(1, 1, length));

		m_effects.push_back({ dash, model, BLM::GLMzero });
	}

	void Update() {
		for (int i(0); i < m_effects.size();) {
			m_effects[i].effect.Update(Time::dt);
			if (m_effects[i].effect.IsDone()) {
				m_effects.erase(m_effects.begin() + i);
				continue;
			}

			++i;
		}
	}

	void Render() {
		for (int i(0); i < m_effects.size(); ++i) {
			m_effects[i].effect.Draw(m_effects[i].model,  m_effects[i].colour);
		}
	}

private:
	static ObjMorphLoader laser;
	static ObjMorphLoader dash;

	struct DrawData
	{
		ObjMorphLoader effect;
		glm::mat4 model;
		glm::vec3 colour;
	};

	std::vector<DrawData> m_effects = {};
};

inline ObjMorphLoader Effects::laser{};
inline ObjMorphLoader Effects::dash{};
