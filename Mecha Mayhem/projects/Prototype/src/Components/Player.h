#pragma once
#include "Utilities/ControllerInput.h"
#include "PhysBody.h"
#include "ObjMorphLoader.h"
#include "Utilities/Time.h"
#include <AudioEngine.h>

class Player
{
public:
	enum class WEAPON {
		FIST,
		SWORD,
		PISTOL,
		RIFLE,
		CANNON,
		MACHINEGUN,
		SHOTGUN,
		MISSILE
	};
	enum class OFFHAND {
		EMPTY = 0,	//continues from WEAPON to allow for rand() choice
		HEALPACK1 = 6,
		HEALPACK2//,
		//SWORD
	};
	// Overloads: (name, ammo, damage, cooldown, movementSpeed, Range)
	struct GunProperties
	{
		WEAPON type;
		short ammoCapacity;
		short damage;
		float cooldown;
		float movementSpeed = 10.f;
		float maxRange = 2000;
	};
	Player() { }

	//inits the UI cam as well as lods the player anims
	static void Init(int width, int height);

	//deletes the static objs
	static void Unload();

	//currently only updates helidrone
	static void Update();

	//sets the UI aspect Ratio
	static void SetUIAspect(int width, int height);

	//camera follow distance
	static void SetCamDistance(float dist) { m_camDistance = dist; }

	//where to players go when they die?
	static void SetSkyPos(glm::vec3 pos) { m_skyPos = pos; }

	//get the weapon Obj, returns pistol if empty
	static ObjLoader& GetWeaponModel(WEAPON choice);
	static ObjMorphLoader& GetOffhandModel(OFFHAND choice);

	/*
	loads with default stats
	0 = dummy
	1 = JJ's
	2 = JL's
	3 = Ryan's
	4 = Bag
	*/
	Player& Init(CONUSER user, int characterModel, const glm::vec3& colour = BLM::GLMzero, short camPos = -1);

	Player& SetColour(const glm::vec3& colour) { m_colour = colour; return *this; }

	//in radians, y is horizontal, x is vertical
	Player& SetRotation(float y, float x) { m_rot = glm::vec2(x, y); return *this; }

	Player& SetSpawn(const glm::vec3& pos) { m_spawnPos = pos; return *this; }


	Player& SetMaxHealth(short amt) { m_maxHealth = amt; if (m_health > amt) m_health = amt; return *this; }

	//sends the animations to the morph animator, so call before perform draw in morph
	//if camNum matches the player's number, draw the UI
	void Draw(const glm::mat4& model, short camNum, short numOfCams, bool paused);

	//update the morph animator and stuff like health
	void Update(PhysBody& body);
	
	//exists to render death better lol
	void LateUpdate(Transform& body);

	//make the player move, returns true if player is currently dead
	void GetInput(PhysBody& body, Transform& head, Transform& personalCam);

	CONUSER GetUser() { return m_user; }

	void ClearWeapons() {
		m_currWeapon = WEAPON::FIST;
		m_secWeapon = WEAPON::FIST;
		m_currWeaponAmmo = 0;
		m_secWeaponAmmo = 0;
		m_offhand = OFFHAND::EMPTY;
		m_meleeDmg = m_punchDmg;
	}

	void GameEnd() {
		MakeInvincible(true);
		GainHealth(100);
		if (!IsAlive())
			m_respawnTimer = Time::dt;
	}

	void MakeInvincible(bool choice) { m_invincible = choice; }

	void GainHealth(short amt) {
		m_health += amt;
		if (m_health > m_maxHealth)
			m_health = m_maxHealth;
	}

	//void GivePoints(int amt) { m_killCount += amt; }

	//returns true if kill shot
	bool TakeDamage(short dmg) {
		if (m_health <= 0 || m_invincible)	return false;

		if ((m_health -= dmg) <= 0) {
			m_health = 0;
			m_respawnTimer = m_respawnDelay;
			
			//do death management in Update()

			return true;
		}
		else {
			//m_hitSound.play();
			m_damageCounter = 0.75f;
		}

		return false;
	}

	bool IsAlive() { return m_respawnTimer == 0; }

	bool IsPlayer() { return m_user != CONUSER::NONE; }

	//returns true if <= than timer
	bool RespawnDelayTest(float timer) { return m_respawnTimer <= timer; }

	//returns true if successful
	bool PickUpWeapon(WEAPON pickup);

	//returns true if successful
	bool PickUpOffhand(OFFHAND pickup);

	bool PickUpSword() {
		if (m_meleeDmg == m_punchDmg) {
			m_meleeDmg = m_swordDmg;

			AudioEngine::Instance().GetEvent("pickup").Restart();

			return true;
		}
		return false;
	}

	bool HasWon(size_t scoreGoal) { return m_killCount >= scoreGoal; }
	short GetScore() { return m_killCount; }

private:
	bool groundTest(float yVelo, PhysBody& bodyPos);

	//attacking
	btVector3 Melee(const glm::vec3& pos);
	void UseWeapon(PhysBody& body, Transform& head, float offset);
	void LaserGun(float offset, Transform& head, short damage, float distance, WEAPON type);

	//other things
	void SwapWeapon();
	void UseHeal();

	const GunProperties& GetProperties(WEAPON type) const;

	//digit 1 is first digit
	/*int GetDigit(int number, int digit) {
		if (digit < 1)	return 0;

		int num = 1;
		for (int i(1); i < digit; ++i)	num *= 10;

		num = number / num;
		return num - (num / 10) * 10;
	}*/

	static const glm::mat4 m_modelOffset;
	static const glm::mat4 m_gunOffsetMat;
	static const glm::mat4 m_swordOffsetMat;
	static constexpr float pi = glm::half_pi<float>() - 0.01f;

	static const glm::vec4 m_gunOffset;
	static const btVector3 m_gravity;
	static glm::vec3 m_skyPos;
	static Camera m_orthoCam;
	static float m_camDistance;
	static float m_dashDistance;

	static Sprite m_healthBarOutline;
	static Sprite m_healthBar;
	static Sprite m_healthBarDamaged;
	static Sprite m_healthBarBack;
	static Sprite m_dashBarOutline;
	static Sprite m_dashBar;
	static Sprite m_dashBarBack;
	static Sprite m_reticle;
	static Sprite m_scoreBack;
	static Sprite m_digits[10];
	static Sprite m_ammoBar;
	static Sprite m_ammoBarBack;
	
	


	static Sprite m_heal2;
	static Sprite m_heal1;
	static Sprite m_fistIcon;
	static Sprite m_pistolIcon;
	static Sprite m_rifleIcon;
	static Sprite m_canonIcon;
	static Sprite m_machineGunIcon;
	static Sprite m_shotgunIcon;

	static Sprite& GetIcon(WEAPON choice);

	static ObjLoader m_pistol;
	static ObjLoader m_rifle;
	static ObjLoader m_canon;
	static ObjLoader m_machineGun;
	static ObjLoader m_shotgun;
	static ObjLoader m_sword;

	static ObjMorphLoader m_heliDrone;
	static ObjMorphLoader m_healPack;

	ObjMorphLoader m_charModel = {};

	CONUSER m_user = CONUSER::NONE;

	std::string m_charModelIndex = "char";

	bool m_drawSelf = true;
	bool m_grounded = false;
	bool m_punched = false;
	bool m_stepped = false;
	bool m_invincible = false;

	static const short m_punchDmg = 8;
	static const short m_swordDmg = 16;

	short m_meleeDmg = m_punchDmg;

	short m_camPos = -1;
	short m_maxHealth = 100;
	short m_health = m_maxHealth;
	short m_killCount = 0;

	short m_currWeaponAmmo = 0;
	short m_secWeaponAmmo = 0;
	WEAPON m_currWeapon = WEAPON::FIST;
	WEAPON m_secWeapon = WEAPON::FIST;

	OFFHAND m_offhand = OFFHAND::EMPTY;

	float m_dashDelay = 15.f;
	float m_dashTimer = 0.f;

	float m_respawnDelay = 5.f;
	float m_respawnTimer = 0.f;

	float m_damageCounter = 0.f;

	float m_dropTimer = 0;
	float m_jumpHeld = 0;
	float m_speed = 10.f;
	float m_weaponCooldown = 0.f;

	//Gun stuff
	static const GunProperties pistol;
	static const GunProperties cannon;
	static const GunProperties rifle;
	static const GunProperties missileLauncher;
	static const GunProperties shotgun;
	static const GunProperties machineGun;
	//static const float shotgunDistance;

	glm::vec3 m_colour = glm::vec3(0.f);

	//glm::quat m_startRot = glm::quat(1, 0, 0, 0);
	glm::vec3 m_spawnPos = glm::vec3(0.f);
	glm::vec3 m_deathPos = glm::vec3(0.f);
	glm::vec2 m_rot = glm::vec2(0.f);
	glm::vec2 m_deathRot = glm::vec2(0.f);
};

