#pragma once
#include "Player.h"
#include "Utilities/Time.h"

class Spawner {
public:
	Spawner() {}

	Spawner& Init(float radius, float delay) {
		m_spawnerModel.LoadMeshs("spawner/spawner", true);
		m_radius = radius;
		m_timer = m_delay = delay;
		m_spawnerModel.SetDirection(false);
		m_replaceTimer = m_replaceDelay;

		return *this;
	}

	void SetBounds(int lower, int upper) {
		if ((lower > upper) || (lower < 0))	return;
		lowerBound = lower;
		upperBound = upper - lower + 1;
	}

	void Render(glm::mat4 model) {
		
		if (m_timer == 0) {
			glm::vec3 colour = BLM::GLMzero;
			if (m_replaceTimer < m_colourPoint) {
				colour = glm::vec3(1.f - m_replaceTimer * m_invColourPoint);
			}

			if (m_currWeapon == Player::WEAPON::FIST)
				Player::GetOffhandModel(Player::OFFHAND::HEALPACK2).Draw(model * m_gunOffset, colour);
			else			Player::GetWeaponModel(m_currWeapon).Draw(model * m_gunOffset, colour);
		}

		m_spawnerModel.Draw(model);
	}

	//return true if colelcted
	void Update(entt::registry* reg, glm::vec3 pos) {
		m_spawnerModel.Update(Time::dt);
		if (m_timer > 0) {
			m_timer -= Time::dt;
			if (m_timer <= 0) {
				m_timer = 0;
				m_spawnerModel.SetDirection(true);
				m_replaceTimer = m_replaceDelay;

				//decide on what spawns
				m_currWeapon = Player::WEAPON(lowerBound + rand() % upperBound);
			}
			return;
		}

		m_replaceTimer -= Time::dt;
		if (m_replaceTimer <= 0) {
			m_replaceTimer = m_replaceDelay;

			//decide on what spawns
			m_currWeapon = Player::WEAPON(lowerBound + rand() % upperBound);
		}

		Player* touched = nullptr;
		float distance = 100;
		reg->view<Player, PhysBody>().each([&](Player& p, PhysBody& body) {
			if (p.IsPlayer() && p.IsAlive()) {
				if (body.TestAABB(pos + BLM::GLMup, m_radius)) {
					float temp = glm::length(pos - BLM::BTtoGLM(body.GetTransform().getOrigin()));
					if (temp <= distance) {
						distance = temp;
						touched = &p;
					}
				}
			}
		});

		if (touched != nullptr) {
			if (m_currWeapon == Player::WEAPON::FIST) {
				if (touched->PickUpOffhand(Player::OFFHAND::HEALPACK2)) {
					m_timer = m_delay;
					m_spawnerModel.SetDirection(false);
				}
			}
			else if (m_currWeapon == Player::WEAPON::SWORD) {
				if (touched->PickUpSword()) {
					m_timer = m_delay;
					m_spawnerModel.SetDirection(false);
				}
			}
			else if (touched->PickUpWeapon(m_currWeapon)) {
				m_timer = m_delay;
				m_spawnerModel.SetDirection(false);
			}
		}
	}

private:
	static const glm::mat4 m_gunOffset;
	static const float m_replaceDelay;
	static const float m_colourPoint;
	static const float m_invColourPoint;

	ObjMorphLoader m_spawnerModel = {};

	//change to int later
	Player::WEAPON m_currWeapon = Player::WEAPON::FIST;

	int lowerBound = 0;
	int upperBound = 6;

	float m_radius = 0.5f;
	float m_timer = 0;
	float m_delay = 5.f;
	float m_replaceTimer = 0.f;
};

inline const float Spawner::m_replaceDelay = 30.f;
inline const float Spawner::m_colourPoint = 5.f;

inline const float Spawner::m_invColourPoint = 1.f / m_colourPoint;
inline const glm::mat4 Spawner::m_gunOffset = glm::mat4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 1, 0, 1
);
