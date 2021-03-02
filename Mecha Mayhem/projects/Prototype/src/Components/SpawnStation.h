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

		return *this;
	}

	void SetBounds(int lower, int upper) {
		if ((lower > upper) || (lower < 0))	return;
		lowerBound = lower;
		if (lower == upper)		upperBound = 0;
		else	upperBound = upper - lower + 1;
	}

	void Render(glm::mat4 model) {
		
		if (m_timer == 0) {
			if (m_currWeapon == Player::WEAPON::FIST)
				Player::GetOffhandModel(Player::OFFHAND::HEALPACK2).Draw(model * m_gunOffset);
			else			Player::GetWeaponModel(m_currWeapon).Draw(model * m_gunOffset);
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
				//decide on what spawns
				switch (lowerBound + rand() % upperBound) {
				case 0:		m_currWeapon = Player::WEAPON::FIST;		break;
				case 1:		m_currWeapon = Player::WEAPON::PISTOL;		break;
				case 2:		m_currWeapon = Player::WEAPON::RIFLE;		break;
				case 3:		m_currWeapon = Player::WEAPON::CANNON;		break;
				case 4:		m_currWeapon = Player::WEAPON::MACHINEGUN;	break;
				case 5:		m_currWeapon = Player::WEAPON::SHOTGUN;		break;
				//case 6:		m_currWeapon = Player::WEAPON::SWORD;		break;
				}
			}
			return;
		}

		reg->view<Player, PhysBody>().each(
			[&](Player& p, PhysBody& body) {
				if (p.IsPlayer())	if (body.TestAABB(pos + BLM::GLMup, m_radius)) {
					if (m_currWeapon == Player::WEAPON::FIST) {
						if (p.PickUpOffhand(Player::OFFHAND::HEALPACK2)) {
							m_timer = m_delay;
							m_spawnerModel.SetDirection(false);
						}
					}
					else if (p.PickUpWeapon(m_currWeapon)) {
						m_timer = m_delay;
						m_spawnerModel.SetDirection(false);
					}
				}
			}
		);
	}

private:
	static const glm::mat4 m_gunOffset;

	ObjMorphLoader m_spawnerModel = {};

	//change to int later
	Player::WEAPON m_currWeapon = Player::WEAPON::FIST;

	int lowerBound = 0;
	int upperBound = 6;

	float m_radius = 0.5f;
	float m_timer = 0;
	float m_delay = 5;
};

inline const glm::mat4 Spawner::m_gunOffset = glm::mat4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 1, 0, 1
);
