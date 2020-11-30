#pragma once
#include "Utilities/ControllerInput.h"
#include "PhysBody.h"
#include "ObjMorphLoader.h"

class Player
{
public:
	enum class WEAPON {
		FIST,
		GUN
	};
	enum class OFFHAND {
		EMPTY,
		HEALPACK
	};

	Player() { }

	//inits the UI cam as well as lods the player anims
	static void Init(int width, int height);

	//sets the UI aspect Ratio
	static void SetUIAspect(int width, int height);

	static void SetCamDistance(float dist) { m_camDistance = dist; }

	/*
	loads with default stats
	0 = dummy
	1 = JJ's
	2 = JL's
	*/
	Player& Init(CONUSER user, int characterModel);

	//in radians
	Player& SetRotation(float y, float x) { m_rot = glm::vec2(x, y); return *this; }

	//sends the animations to the morph animator, so call before perform draw in morph
	//if camNum matches the player's number, draw the UI
	void Draw(const glm::mat4& model, short camNum, short numOfCams);

	//update the morph animator and stuff like health
	void Update(PhysBody& body);

	//make the player move
	void GetInput(PhysBody& body, Transform& head, Transform& personalCam);

	//returns true if kill shot
	bool TakeDamage(short dmg) {
		if (m_health == 0)	return false;

		if ((m_health -= dmg) <= 0) {
			m_health = 0;
			m_respawnTimer = m_respawnDelay;
			m_charModel.BlendTo(m_charModelIndex + "/death", 0.25f);
			return true;
		}
		return false;
	}


private:
	void UseWeapon(PhysBody& body);
	void PickUpWeapon(WEAPON pickup);
	void PickUpOffhand(OFFHAND pickup);
	
	void ShootDash(float width, glm::vec3 pos, glm::quat rotation);

	void updateDash();

	//digit 1 is first digit
	int GetDigit(int number, int digit) {
		if (digit < 1)	return 0;

		int num = 1;
		for (int i(1); i < digit; ++i)	num *= 10;

		num = number / num;
		return num - (num / 10) * 10;
	}

	entt::entity Dash = entt::null;

	static const glm::mat4 m_modelOffset;
	static Camera m_orthoCam;
	static constexpr float pi = glm::half_pi<float>() - 0.01f;
	static float m_camDistance;
	static float m_dashDistance;

	CONUSER m_user = CONUSER::NONE;

	ObjMorphLoader m_charModel = ObjMorphLoader("char/idle", true);

	std::string m_charModelIndex = "char";

	bool m_drawSelf = true;
	bool m_grounded = false;
	bool m_punched = false;

	short m_maxHealth = 20;
	short m_health = m_maxHealth;
	short m_killCount = 0;

	//float m_dashDelay = 30.f;
	float m_dashDelay = 1.f;
	float m_dashTimer = 0.f;

	float m_respawnDelay = 5.f;
	float m_respawnTimer = 0.f;

	float m_jumpHeld = 0;
	float m_speed = 15.f;
	float m_weaponCooldown = 0.f;

	glm::quat m_startRot = glm::quat(1, 0, 0, 0);
	glm::vec3 m_spawnPos = glm::vec3(0.f);
	glm::vec2 m_rot = glm::vec2(0.f);

	WEAPON m_currWeapon = WEAPON::FIST;
	WEAPON m_secWeapon = WEAPON::FIST;
	OFFHAND m_offhand = OFFHAND::EMPTY;

	Sprite m_healthBarOutline{ "healthbar.png", 15.96f, 1.5f };
	Sprite m_healthBar{ glm::vec4(0, 0, 1, 1.f), 14.95f, 0.9f };
	Sprite m_healthBarBack{ glm::vec4(0, 0, 0, 1.f), 14.95f, 0.9f };

	Sprite m_dashBarOutline{ "energybar.png", 10.38, 1.5f };
	Sprite m_dashBar{ glm::vec4(1, 1, 1, 1.f), 9.15f, 0.5f };
	Sprite m_dashBarBack{ glm::vec4(0, 0, 0, 1.f), 9.15f, 0.5f };
	//Sprite m_healthBar{ "Diorama Texture.png", 15, 1 };
	//Sprite m_dashBar{ "Diorama Texture.png", 10, 0.75f };
};

