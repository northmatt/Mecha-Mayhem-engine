#pragma once
#include "Player.h"
#include "Utilities/Time.h"

class Spawner {
public:
	Spawner() {}

	void Init(float radius, float delay) {
		m_spawnerModel = { "spawner/spawner", true };
		m_radius = radius;
		m_timer = m_delay = delay;
		m_currWeapon = Player::WEAPON(rand() % 6 + 1);
		m_spawnerModel.SetDirection(false);
	}

	void Render(glm::mat4 model) {
		
		if (m_timer == 0) {
			switch (m_currWeapon) {
			case Player::WEAPON::CANON:			Player::m_canon.Draw(model * m_gunOffset);		break;
			case Player::WEAPON::PISTOL:		Player::m_pistol.Draw(model * m_gunOffset);		break;
			case Player::WEAPON::RIFLE:			Player::m_rifle.Draw(model * m_gunOffset);		break;
			case Player::WEAPON::SWORD:			Player::m_sword.Draw(model * m_gunOffset);		break;
			case Player::WEAPON::MACHINEGUN:	Player::m_machineGun.Draw(model * m_gunOffset);	break;
			case Player::WEAPON::SHOTGUN:		Player::m_shotgun.Draw(model * m_gunOffset);	break;
			default:							Player::m_pistol.Draw(model * m_gunOffset);		break;
			}
		}

		m_spawnerModel.Draw(model);
	}

	//return true if colelcted
	void Update(entt::registry* reg, glm::vec3 pos) {
		if (m_timer > 0) {
			m_timer -= Time::dt;
			if (m_timer <= 0) {
				m_timer = 0;
				m_spawnerModel.SetDirection(false);
				m_currWeapon = Player::WEAPON(rand() % 6 + 1);
			}
			return;
		}

		reg->view<Player, PhysBody>().each(
			[&](Player& p, PhysBody& body) {
				if (p.IsPlayer()) {
					if (body.TestAABB(pos + BLM::GLMup, m_radius)) {
						if (p.PickUpWeapon(m_currWeapon)) {
							m_timer = m_delay;
							m_spawnerModel.SetDirection(true);
						}
					}
				}
			}
		);
	}

private:
	static const glm::mat4 m_gunOffset;

	ObjMorphLoader m_spawnerModel;

	Player::WEAPON m_currWeapon;

	float m_radius = 1;
	float m_timer = 0;
	float m_delay = 5;
};

inline const glm::mat4 Spawner::m_gunOffset = glm::mat4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 1, 0, 1
);
