#pragma once
#include "Utilities/ControllerInput.h"
#include "PhysBody.h"
#include "ObjMorphLoader.h"
#include "SoundSystem.h"

class Player
{
public:
	enum class WEAPON {
		FIST,
		GUN
	};
	enum class OFFHAND {
		EMPTY,
		HEALPACK1,
		HEALPACK2
	};

	Player() { }

	//inits the UI cam as well as lods the player anims
	static void Init(int width, int height);

	//sets the UI aspect Ratio
	static void SetUIAspect(int width, int height);

	//camera follow distance
	static void SetCamDistance(float dist) { m_camDistance = dist; }

	//where to players go when they die?
	static void SetSkyPos(glm::vec3 pos) { m_skyPos = pos; }

	/*
	loads with default stats
	0 = dummy
	1 = JJ's
	2 = JL's
	*/
	Player& Init(CONUSER user, int characterModel);

	//in radians
	Player& SetRotation(float y, float x) { m_rot = glm::vec2(x, y); return *this; }

	Player& SetSpawn(const glm::vec3 pos) { m_spawnPos = pos; return *this; }

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
			return true;
		}
		return false;
	}


private:
	void UseWeapon(PhysBody& body, Transform& head);
	void UseHeal();

	//returns true if successful
	bool PickUpWeapon(WEAPON pickup);

	//returns true if successful
	bool PickUpOffhand(OFFHAND pickup);

	//digit 1 is first digit
	int GetDigit(int number, int digit) {
		if (digit < 1)	return 0;

		int num = 1;
		for (int i(1); i < digit; ++i)	num *= 10;

		num = number / num;
		return num - (num / 10) * 10;
	}

	static const glm::mat4 m_modelOffset;
	static glm::vec3 m_skyPos;
	static Camera m_orthoCam;
	static constexpr float pi = glm::half_pi<float>() - 0.01f;
	static float m_camDistance;
	static float m_dashDistance;
	static Sound2D m_shootLaser;
	static Sound2D m_hitSound;
	static Sound2D m_swapWeapon;
	static Sound2D m_walk[5];

	static Sprite m_healthBarOutline;
	static Sprite m_healthBar;
	static Sprite m_healthBarBack;
	static Sprite m_dashBarOutline;
	static Sprite m_dashBar;
	static Sprite m_dashBarBack;

	CONUSER m_user = CONUSER::NONE;

	ObjMorphLoader m_charModel = { "char/idle", true };

	std::string m_charModelIndex = "char";

	bool m_drawSelf = true;
	bool m_grounded = false;
	bool m_punched = false;
	bool m_stepped = false;

	short m_maxHealth = 20;
	short m_health = m_maxHealth;
	short m_killCount = 0;

	short m_currWeaponAmmo = 0;
	short m_secWeaponAmmo = 100;
	WEAPON m_currWeapon = WEAPON::FIST;
	WEAPON m_secWeapon = WEAPON::GUN;

	OFFHAND m_offhand = OFFHAND::EMPTY;

	float m_dashDelay = 0.f;
	float m_dashTimer = 0.f;

	float m_respawnDelay = 5.f;
	float m_respawnTimer = 0.f;

	float m_jumpHeld = 0;
	float m_speed = 15.f;
	float m_weaponCooldown = 0.f;

	glm::quat m_startRot = glm::quat(1, 0, 0, 0);
	glm::vec3 m_spawnPos = glm::vec3(0.f);
	glm::vec3 m_deathPos = glm::vec3(0.f);
	glm::vec2 m_rot = glm::vec2(0.f);
};

