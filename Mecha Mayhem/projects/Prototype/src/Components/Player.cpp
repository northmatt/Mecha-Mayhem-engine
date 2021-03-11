#include "Player.h"
#include "Utilities/Rendering.h"
#include "ECS.h"

const btVector3 Player::m_gravity = btVector3(0, -100, 0);

const glm::vec4 Player::m_gunOffset = glm::vec4(
	0.3f, -0.3f, -0.4f, 1
);

const glm::mat4 Player::m_gunOffsetMat = glm::mat4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0.3f, 0.1f, -0.3f, 1
);

const glm::mat4 Player::m_swordOffsetMat = glm::mat4(
	0, 0, 1, 0,
	glm::sin(glm::radians(140.f)), glm::cos(glm::radians(140.f)), 0, 0,
	-glm::cos(glm::radians(140.f)), glm::sin(glm::radians(140.f)), 0, 0,
	-0.2f, 0.4f, 0.4f, 1
);

const glm::mat4 Player::m_modelOffset = glm::mat4(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, -1, 0, 0
);
//Gun stuff												
														//type,				ammo,	damage, cooldown,	movementSpeed,  maxRange (default 2000)
const Player::GunProperties Player::pistol			{ WEAPON::PISTOL,		30,		25,		0.4f,		 10.f,				50.f };
const Player::GunProperties Player::rifle			{ WEAPON::RIFLE,		7,		80,		3.5f,		 10.f };
const Player::GunProperties Player::cannon			{ WEAPON::CANNON,		3,		50,		2.f,		 10.f };
const Player::GunProperties Player::missileLauncher	{ WEAPON::MISSILE,		1,		100,	3.f,		 10.f };
const Player::GunProperties Player::shotgun			{ WEAPON::SHOTGUN,		10,		30,		2.f,		 10.f,		25.f };
const Player::GunProperties Player::machineGun		{ WEAPON::MACHINEGUN,	50,		7,		0.1f,		 10.f };

float Player::m_camDistance = 5.f;
float Player::m_dashDistance = 7.5f;
glm::vec3 Player::m_skyPos = glm::vec3(0, 50, 0);

Camera Player::m_orthoCam = {};
Sprite Player::m_healthBarOutline = {};
Sprite Player::m_healthBar = {};
Sprite Player::m_healthBarDamaged = {};
Sprite Player::m_healthBarBack = {};
Sprite Player::m_dashBarOutline = {};
Sprite Player::m_dashBar = {};
Sprite Player::m_dashBarBack = {};
Sprite Player::m_reticle = {};
Sprite Player::m_scoreBack = {};
Sprite Player::m_digits[10] = {};
Sprite Player::m_ammoBar = {};
Sprite Player::m_ammoBarBack = {};


Sprite Player::m_heal2 = {};
Sprite Player::m_heal1 = {};
Sprite Player::m_fistIcon = {};
Sprite Player::m_pistolIcon = {};
Sprite Player::m_rifleIcon = {};
Sprite Player::m_canonIcon = {};
Sprite Player::m_machineGunIcon = {};
Sprite Player::m_shotgunIcon = {};

ObjLoader Player::m_pistol = {};
ObjLoader Player::m_canon = {};
ObjLoader Player::m_rifle = {};
ObjLoader Player::m_machineGun = {};
ObjLoader Player::m_shotgun = {};
ObjLoader Player::m_sword = {};
ObjMorphLoader Player::m_heliDrone = {};
ObjMorphLoader Player::m_healPack = {};

void Player::Init(int width, int height)
{
	m_orthoCam.SetOrthoHeight(10).SetPosition(BLM::GLMzero).SetNear(-10).Setfar(10)
		.SetIsOrtho(true).ResizeWindow(width, height);

	m_pistol.LoadMesh("models/Pistol.obj", true);
	m_canon.LoadMesh("models/MissileLauncher.obj", true);
	m_rifle.LoadMesh("models/Rifle.obj", true);
	m_machineGun.LoadMesh("models/MachineGun.obj", true);
	m_shotgun.LoadMesh("models/Shotgun.obj", true);
	m_sword.LoadMesh("models/Sword.obj", true);

	m_heliDrone.LoadMeshs("drone/Heli", true);
	m_healPack.LoadMeshs("healing", true);

	/*m_shootLaser = { "laser.mp3", "shooting" };
	m_shootLaser.setGroupVolume(0.5f);
	m_hitSound = { "oof.mp4.mp3", "sfx" };
	m_deathSound = { "oof.mp4.mp3", "death" };
	m_deathSound.setGroupVolume(1.5f);
	m_swapWeapon = { "Equip.mp3", "sfx" };
	m_walk[0] = { "MetalFloor/1StepNoise.mp3", "walking" };
	m_walk[1] = { "MetalFloor/2StepNoise.mp3", "walking" };
	m_walk[2] = { "MetalFloor/3StepNoise.mp3", "walking" };
	m_walk[3] = { "MetalFloor/4StepNoise.mp3", "walking" };
	m_walk[4] = { "MetalFloor/5StepNoise.mp3", "walking" };
	m_walk[0].setGroupVolume(0.25f);
	m_wiff = { "PunchWiff.mp3", "sfx" };
	m_punch = { "MetalFloor/3StepNoise.mp3", "sfx" };*/

	m_healthBarOutline = { "ui/healthbar.png", 15.96f, 1.5f };
	m_healthBar = { glm::vec4(1, 0, 0, 1.f), 14.95f, 0.9f };
	m_healthBarDamaged = { glm::vec4(1, 1, 1, 1.f), 14.95f, 0.9f };
	m_healthBarBack = { glm::vec4(0, 0, 0, 1.f), 14.95f, 0.9f };

	m_dashBarOutline = { "ui/energybar.png", 10.38f, 1.5f };
	m_dashBar = { glm::vec4(0.5f, 0.5f, 1, 1.f), 9.15f, 0.5f };
	m_dashBarBack = { glm::vec4(0, 0, 0, 1.f), 9.15f, 0.5f };

	m_ammoBar = { glm::vec4(1, 1, 1, 1.f), 0.2f, 2.f };
	m_ammoBarBack = { glm::vec4(0, 0, 0, 1.f), 0.25f, 2.05f };
	

	m_reticle = { "ui/reticle.png", 1.f, 1.f };
	m_scoreBack = { "ui/ScoreBox.png", 3.f, 3.223f };
	for (int i(0); i < 10; ++i)	m_digits[i] = { "num/" + std::to_string(i) + ".png", 1.25f, 1.6667f };

	m_heal2 = { "ui/heal2.png", 2.f, 1.70f };
	m_heal1 = { "ui/heal1.png", 2.f, 1.70f };
	m_fistIcon = { "ui/empty.png", 2.f, 1.48f };
	m_pistolIcon = { "ui/pistol.png", 2.f, 1.48f };
	m_rifleIcon = { "ui/rifle.png", 2.f, 1.48f };
	m_canonIcon = { "ui/cannon.png", 2.f, 1.48f };
	m_machineGunIcon = { "ui/machineGun.png", 2.f, 1.48f };
	m_shotgunIcon = { "ui/shotgun.png", 2.f, 1.48f };

	ObjMorphLoader("dummy/idle", true).LoadMeshs("dummy/death", true);
	ObjMorphLoader("char/idle", true).LoadMeshs("char/walk", true)
		.LoadMeshs("char/air", true).LoadMeshs("char/death", true)
		.LoadMeshs("char/punch", true);
	ObjMorphLoader("char2/idle", true).LoadMeshs("char2/walk", true)
		.LoadMeshs("char2/air", true).LoadMeshs("char2/death", true)
		.LoadMeshs("char2/punch", true);
	ObjMorphLoader("char3/idle", true).LoadMeshs("char3/walk", true)
		.LoadMeshs("char3/air", true).LoadMeshs("char3/death", true)
		.LoadMeshs("char3/punch", true);
	ObjMorphLoader("char4/idle", true).LoadMeshs("char4/walk", true)
		.LoadMeshs("char4/air", true).LoadMeshs("char4/death", true)
		.LoadMeshs("char4/punch", true);
}

void Player::Unload()
{
	m_heliDrone.Destroy();
	m_healPack.Destroy();
}

void Player::Update()
{
	m_heliDrone.Update(Time::dt);
	m_healPack.Update(Time::dt);
}

void Player::SetUIAspect(int width, int height)
{
	m_orthoCam.ResizeWindow(width, height);
}

Player& Player::Init(CONUSER user, int characterModel, const glm::vec3& colour, short camPos)
{
	m_user = user;
	m_camPos = camPos;
	m_colour = colour;

	switch (characterModel) {
		//dummy (case 0 is also here)
	default:	m_charModelIndex = "dummy";	m_charModel.LoadMeshs("dummy/idle", true);	m_user = CONUSER::NONE;	break;
		//JJ's
	case 1:		m_charModelIndex = "char";	m_charModel.LoadMeshs("char/idle", true);	break;
		//JL's
	case 2:		m_charModelIndex = "char2";	m_charModel.LoadMeshs("char2/idle", true);	break;
		//Ryan's
	case 3:		m_charModelIndex = "char3";	m_charModel.LoadMeshs("char3/idle", true);	break;
		//Bag
	case 4:		m_charModelIndex = "char4";	m_charModel.LoadMeshs("char4/idle", true);	break;
	}

	return *this;
}

void Player::Draw(const glm::mat4& model, short camNum, short numOfCams, bool paused)
{
	glm::mat4 tempModel = model;

	if (m_camPos == camNum) {
		if (m_charModelIndex == "char4")
			tempModel = glm::scale(model, glm::vec3(0.75f, 1.f, 0.75f));

		//draw ui
		float healthPercent = float(m_health) / m_maxHealth;
		float dashPercent = m_dashTimer / m_dashDelay,
			x = 15.7777f,	//1.77 * 10 - 2
			y = 7.5f,
			x2 = 15.7777f,	//1.77 * 10 - 2
			//x2 = 15.9277f,	//1.77 * 10 - 1.85
			y2 = 4.5f,		//places weapons below score
			x3 = 12.7777f,	//x - 3
			y3 = 7.5f;		//places heal besides score

			//x4 x2 - 1.5
		float x4 = 14.2777f;

		float scaleMod = 1.f;

		if (paused && numOfCams > 2) {
			//x = 12.f;
			//x2 = 15.f;
			//x3 = 9.f;
			//x4 = 14.f;
			y = 5.f;
			y2 = 2.f; // -2.5f
			y3 = 5.f;
		}
		if (numOfCams == 2) {
			x = 6.88889f;	//0.88 * 10 - 2
			x2 = 6.88889f;	//0.88 * 10 - 2
			//x2 = 7.03889f;	//0.88 * 10 - 1.85
			x3 = 3.88889f;	//x - 3
			x4 = 5.38889f; // x2 - 1.5
		}
		if (camNum % 2 == 0) { x = -x;	x2 = -x2;	x3 = -x3;	x4 = -x4; }
		if (camNum < 2 && numOfCams > 2) { y = -y;	y2 = -y2;	y3 = -y3; }

		glm::mat4 VP = m_orthoCam.GetViewProjection();

		glm::mat4 UIMat = BLM::GLMMat;

		if (numOfCams > 2) 
		{
			UIMat *= 1.25f;
			scaleMod = 1.25f;
		}

		UIMat[3] = glm::vec4(x, y, -9.9f, 1);
		m_scoreBack.DrawToUI(VP, UIMat, camNum);

		UIMat[3] = glm::vec4(x, y, -10, 1);
		if (m_killCount < 10) {
			m_digits[m_killCount].DrawToUI(VP, UIMat, camNum);
		}
		else if (m_killCount < 100) {
			int digit2 = m_killCount / 10;

			UIMat[3][0] = x - 0.65f * scaleMod;
			m_digits[m_killCount - digit2 * 10].DrawToUI(VP, UIMat, camNum);

			UIMat[3][0] = x + 0.65f * scaleMod;
			m_digits[digit2].DrawToUI(VP, UIMat, camNum);
		}
		else {
			UIMat[3][0] = x - 0.65f * scaleMod;
			m_digits[0].DrawToUI(VP, UIMat, camNum);

			UIMat[3][0] = x + 0.65f * scaleMod;
			m_digits[0].DrawToUI(VP, UIMat, camNum);
		}


		UIMat[3] = glm::vec4(x2 - 0.4f, y2 + 0.25f, -9.9f, 1);
		GetIcon(m_secWeapon).DrawToUI(VP, UIMat, camNum);

		UIMat[3] = glm::vec4(x2 + 0.1f, y2 - 0.25f, -10.f, 1);
		GetIcon(m_currWeapon).DrawToUI(VP, UIMat, camNum);

		if (m_currWeapon != WEAPON::FIST)
		{
			float ammoPercentage = (float)m_currWeaponAmmo / GetProperties(m_currWeapon).ammoCapacity;

			UIMat[3] = glm::vec4(x4, y2, -9.9f, 1);
			m_ammoBarBack.DrawToUI(VP, UIMat, camNum);
			
			UIMat[3] = glm::vec4(x4, y2 - (1.f - ammoPercentage) * scaleMod, -10.f, 1);
			m_ammoBar.SetHeight(2.f * ammoPercentage);
			m_ammoBar.DrawToUI(VP, UIMat, camNum);
		}

		//healpack
		if (m_offhand != OFFHAND::EMPTY) {
			UIMat[3] = glm::vec4(x3, y3, -10.f, 1);
			if (m_offhand == OFFHAND::HEALPACK2)	m_heal2.DrawToUI(VP, UIMat, camNum);
			else if (m_offhand == OFFHAND::HEALPACK1)	m_heal1.DrawToUI(VP, UIMat, camNum);
		}

		UIMat = BLM::GLMMat;

		UIMat[3] = glm::vec4((1 - healthPercent) * 7.475f, -8.5f, -9.9f, 1);
		if (m_damageCounter) 
		{
			m_healthBarDamaged.SetWidth(14.95f * healthPercent);
			m_healthBarDamaged.DrawToUI(VP, UIMat, camNum);
		}
		else 
		{
			m_healthBar.SetWidth(14.95f * healthPercent);
			m_healthBar.DrawToUI(VP, UIMat, camNum);
		}

		UIMat[3] = glm::vec4(0, -8.5f, -9.8f, 1);
		m_healthBarBack.DrawToUI(VP, UIMat, camNum);

		UIMat[3] = glm::vec4(dashPercent * 4.575f, -7.25f, -9.9f, 1);
		m_dashBar.SetWidth(9.15f * (1 - dashPercent));
		m_dashBar.DrawToUI(VP, UIMat, camNum);

		UIMat[3] = glm::vec4(0, -7.25f, -9.8f, 1);
		m_dashBarBack.DrawToUI(VP, UIMat, camNum);


		//drawn last cause layering
		UIMat[3] = glm::vec4(0, -8.5f, -10.f, 1);
		m_healthBarOutline.DrawToUI(VP, UIMat, camNum);

		UIMat[3] = glm::vec4(0, -7.25f, -10.f, 1);
		m_dashBarOutline.DrawToUI(VP, UIMat, camNum);


		if (!paused && IsAlive()) {
			UIMat[3] = glm::vec4(0, 0, -10, 1);
			m_reticle.DrawToUI(VP, UIMat, camNum);
		}

		//is false when cam is too close
		if (!m_drawSelf)	return;
	}
	if (m_meleeDmg == m_swordDmg) {
		if (m_punched)		m_sword.DrawTemp(model * m_gunOffsetMat);
		else		m_sword.DrawTemp(model * m_swordOffsetMat);
	}
	if (!m_punched && m_currWeapon != WEAPON::FIST) {
		GetWeaponModel(m_currWeapon).DrawTemp(model * m_gunOffsetMat);
	}
	m_charModel.DrawTemp(tempModel + m_modelOffset, m_colour - glm::vec3(m_damageCounter));
	if (m_respawnTimer > 0) {
		m_heliDrone.DrawTemp(model - m_modelOffset);
	}

}

void Player::Update(PhysBody& body)
{
	if (m_user == CONUSER::NONE) {
		//rotate dummies once (rot.x is not used in dummies)
		if (m_rot.x == 0) {
			body.SetRotation(glm::angleAxis(-m_rot.y, BLM::GLMup));
			m_rot.x = 1.f;
		}
	}
	m_charModel.Update(Time::dt);

	//when dead
	if (m_respawnTimer > 0) {
		m_charModel.BlendTo(m_charModelIndex + "/death", 0.25f);
		if (m_respawnTimer == m_respawnDelay) {
			m_drawSelf = true;

			//fix rotation because deathlerp
			while (m_rot.y > pi * 2)	m_rot.y -= pi * 4;
			while (m_rot.y < -pi * 2)	m_rot.y += pi * 4;

			//store position of death
			m_deathPos = BLM::BTtoGLM(body.GetTransform().getOrigin());
			m_deathRot = glm::vec2(-pi, m_rot.y + glm::radians(rand() % 360 - 179.5f));

			//stop falling
			body.SetGravity(BLM::BTzero);

			//put somewhere inaccessible
			body.SetPosition(btVector3(0, 1000, 0));

			ClearWeapons();
			//m_deathSound.play();
		}

		m_respawnTimer -= Time::dt;
		if (m_respawnTimer <= 0) {
			m_charModel.BlendTo(m_charModelIndex + "/idle");
			m_respawnTimer = 0;

			//reset stats
			m_health = m_maxHealth;
			m_dashTimer = 0.f;
			m_damageCounter = 0.f;
			m_weaponCooldown = 0.f;
			m_punched = false;
			body.SetPosition(m_spawnPos);
			body.SetGravity(m_gravity);
			body.SetAwake();
		}
		return;
	}

	if (m_dashTimer > 0) {
		m_dashTimer -= Time::dt;
		if (m_dashTimer <= 0)
			m_dashTimer = 0;
	}

	if (m_weaponCooldown > 0) {
		m_weaponCooldown -= Time::dt;
		if (m_weaponCooldown <= 0)
			m_weaponCooldown = 0;
	}

	if (m_damageCounter > 0) {
		m_damageCounter -= Time::dt;
		if (m_damageCounter <= 0)
			m_damageCounter = 0;
	}
}

//lol is to avoid overwriting existing function lol
template<class T>
T fakeSmoothStep(T a, T b, float percent) {
	percent = glm::smoothstep(0.f, 1.f, percent);
	return (1 - percent) * a + percent * b;
}

void Player::LateUpdate(Transform& body)
{
	//since we're editing the transform component later, the physicsbody can be disconnected
	if (m_respawnTimer > 0) {
		float percent = m_respawnTimer / m_respawnDelay;

		body.SetPosition(fakeSmoothStep(m_skyPos, m_deathPos, percent));

		body.SetRotation(glm::angleAxis(
			-fakeSmoothStep(m_deathRot.y, m_rot.y, glm::clamp(percent * 2.f - 0.5f, 0.f, 1.f)),
			BLM::GLMup));
	}
}

void Player::GetInput(PhysBody& body, Transform& head, Transform& personalCam)
{
	//dont care if not a player or dead
	if (m_user == CONUSER::NONE)	return;
	if (m_health == 0) {
		float percent = m_respawnTimer / m_respawnDelay;

		head.SetRotation(glm::angleAxis(fakeSmoothStep(m_deathRot.x, m_rot.x, percent), glm::vec3(1, 0, 0)));
		personalCam.SetPosition(glm::vec3(0, 0, m_camDistance + 1.f - percent));
		return;
	}

	float rayOff = 0;

	//Camera Rotation (above punch for user experience)
	{
		float multiplier = 2.f * (1 - ControllerInput::GetLT(m_user)) + 1.f;
		m_rot.x += ControllerInput::GetRY(m_user) * multiplier * 0.75f * Time::dt;
		m_rot.y += ControllerInput::GetRX(m_user) * multiplier * Time::dt;

		//clamping vertical axis
		if (m_rot.x > pi)			m_rot.x = pi;
		else if (m_rot.x < -pi)		m_rot.x = -pi;
		body.SetRotation(glm::angleAxis(-m_rot.y, BLM::GLMup));
		head.SetRotation(glm::angleAxis(m_rot.x, glm::vec3(1, 0, 0)));


		//camera distance checks

		glm::vec3 rayPos = head.GetGlobalPosition();
		btVector3 test = PhysBody::GetRaycast(rayPos, head.GetForwards() * (m_camDistance * 50.f));
		float distance = m_camDistance;
		float xPos = 0.4f;
		m_drawSelf = true;

		if (m_rot.x < pi * 0.25f) {
			if (test != btVector3()) {
				distance = glm::length(rayPos - BLM::BTtoGLM(test));

				if (distance > m_camDistance) {
					distance = m_camDistance;
					rayOff = -0.0075f;
					//personalCam.SetPosition(glm::vec3(0.4f, 0, m_camDistance));
				}
				else {
					rayOff = -0.0075f * (distance / m_camDistance);
					//personalCam.SetPosition(glm::vec3(0.4f, 0, distance));
				}
			}
			else {
				distance = m_camDistance;
				rayOff = -0.0075f;
				//personalCam.SetPosition(glm::vec3(0.4f, 0, m_camDistance * multiplier * 0.333f));
			}
		}
		else if (m_rot.x < pi * 0.5f) {
			float t = (m_rot.x / pi - 0.25f) * 4;
			rayOff = -0.0075f * (1 - t);
			distance = (1 - t) * m_camDistance - t * 0.5f;
			xPos = 0.4f * (1 - t);
			//personalCam.SetPosition(glm::vec3(0.4f * (1 - t), 0, ((1 - t) * m_camDistance - t * 0.5f) * multiplier * 0.333f));

			m_drawSelf = t < 0.5f;
		}
		else {
			//personalCam.SetPosition(glm::vec3(0, 0, 0.5f * multiplier * 0.333f));
			distance = 0.5f;
			xPos = 0;
		}
		if (!ControllerInput::GetButton(BUTTON::B, m_user))
			distance *= multiplier * 0.333f;
		if (m_drawSelf)		m_drawSelf = (distance > 0.75f);
		personalCam.SetPosition(glm::vec3(xPos, 0, distance));
	}

	if (m_punched) {
		m_charModel.BlendTo(m_charModelIndex + "/punch");

		if (m_charModel.IsDone()) {
			m_charModel.BlendTo(m_charModelIndex + "/idle");
			m_punched = false;
		}
		else	return;
	}

	//controls
	{
		glm::vec3 vel = glm::vec3(0.f);
		vel.y = body.SetAwake().GetVelocity().y();

		vel.x += ControllerInput::GetLX(m_user);
		vel.z -= ControllerInput::GetLY(m_user);

		if (groundTest(vel.y, body)) {
			vel.y = 0;

			//jump
			//if (ControllerInput::GetButtonDown(BUTTON::A, m_user)) {
			if (ControllerInput::GetButton(BUTTON::A, m_user)) {
				vel.y = 15.f;
				m_jumpHeld = Time::dt;
			}
			else m_jumpHeld = 0;

			if (m_punched = ControllerInput::GetButtonDown(BUTTON::X, m_user)) {
				//punch only when on ground
				body.SetVelocity(BLM::BTtoGLM(
					Melee(BLM::BTtoGLM(body.GetTransform().getOrigin()) - head.GetForwards())
				));
				return;
			}
			else {
				//ground anims
				if (vel.x != 0 || vel.z != 0) {
					m_charModel.SetSpeed(std::max(fabsf(vel.x), fabsf(vel.z))).
						BlendTo(m_charModelIndex + "/walk");
					if (m_charModel.Getp0() == 0 || m_charModel.Getp0() == 4) {
						if (!m_stepped) {

							//m_walk[rand() % 5].play();
							AudioEngine::Instance().GetEvent("step").Restart();

							m_stepped = true;
						}
					}
					else	m_stepped = false;
				}
				else
					m_charModel.BlendTo(m_charModelIndex + "/idle");
			}
		}
		else {
			if (m_jumpHeld > 0 && m_jumpHeld < 0.12f) {
				if (ControllerInput::GetButton(BUTTON::A, m_user)) {
					vel.y = 15.f;
					m_jumpHeld += Time::dt;
				}
				else m_jumpHeld = 0;
			}
			m_charModel.BlendTo(m_charModelIndex + "/air");
		}

		//dash and other stuff
		{
			if (ControllerInput::GetButtonDown(BUTTON::B, m_user)) {
				//this is for testing
				/*if (m_offhand == OFFHAND::EMPTY)
					TakeDamage(1);*/
				if (m_offhand == OFFHAND::HEALPACK1 || m_offhand == OFFHAND::HEALPACK2)
					UseHeal();
			}

			//dash if moving and velocity adjustment
			if (vel.x != 0 || vel.z != 0) {
				glm::vec3 normalized = glm::normalize(glm::vec3(vel.x, 0, vel.z));
				float percent = std::clamp(fabsf(vel.x) + fabsf(vel.z), 0.f, 1.f);
				vel.x = normalized.x * m_speed * percent;
				vel.z = normalized.z * m_speed * percent;

				vel = glm::vec4(vel, 1) * glm::rotate(BLM::GLMMat, m_rot.y, BLM::GLMup);

				if (m_dashTimer == 0) {
					//if (ControllerInput::GetButtonDown(BUTTON::RB, m_user)) {
					if (ControllerInput::GetButton(BUTTON::LB, m_user)) {
						m_dashTimer = m_dashDelay;
						glm::vec3 ogPos = BLM::BTtoGLM(body.GetTransform().getOrigin());
						btVector3 pos = PhysBody::GetRaycastWithDistanceLimit(ogPos,
							glm::vec3(vel.x, 0, vel.z) * m_dashDistance * 10.f, m_dashDistance);

						glm::vec3 newPos = BLM::BTtoGLM(pos);
						Rendering::effects->ShootDash(glm::rotation(
							glm::normalize(glm::vec3(vel.x, 0, -vel.z)), glm::vec3(0, 0, 1)),
							newPos, glm::length(newPos - ogPos));
						body.SetPosition(pos);
					}
				}
			}
			//dash without moving
			else if (m_dashTimer == 0) {
				//if (ControllerInput::GetButtonDown(BUTTON::RB, m_user)) {
				if (ControllerInput::GetButton(BUTTON::LB, m_user)) {
					m_dashTimer = m_dashDelay;
					glm::vec3 ogPos = BLM::BTtoGLM(body.GetTransform().getOrigin());
					btVector3 pos = PhysBody::GetRaycastWithDistanceLimit(ogPos,
						glm::vec4(0, 0, m_dashDistance * -50.f, 1)
						* glm::rotate(BLM::GLMMat, m_rot.y, BLM::GLMup), m_dashDistance);

					glm::vec3 newPos = BLM::BTtoGLM(pos);
					Rendering::effects->ShootDash(BLM::BTtoGLM(body.GetTransform().getRotation()),
						newPos, glm::length(newPos - ogPos));
					body.SetPosition(pos);
				}
			}
		}
		body.SetVelocity(vel);
	}

	//guns
	{
		if (m_currWeapon != WEAPON::FIST) {
			if (ControllerInput::GetButton(BUTTON::RB, m_user)) {
				//so that you only drop one gun
				if (ControllerInput::GetButtonDown(BUTTON::RB, m_user))
					m_dropTimer = 0.25f + Time::dt;

				if (m_dropTimer > 0) {
					m_dropTimer -= Time::dt;
					if (m_dropTimer <= 0) {
						m_dropTimer = 0.f;

						m_currWeaponAmmo = 0;
						SwapWeapon();
					}
				}
			}
			else m_dropTimer = 0.f;
		}
		if (m_secWeapon != WEAPON::FIST) if (ControllerInput::GetButtonDown(BUTTON::Y, m_user)) {
			SwapWeapon();
		}
		if (m_weaponCooldown == 0) if (ControllerInput::GetRTRaw(m_user)) {
			UseWeapon(body, head, rayOff);
		}
	}

	return;
}

//used this: https://gamedev.stackexchange.com/questions/58012/detect-when-a-bullet-rigidbody-is-on-ground
bool Player::groundTest(float yVelo, PhysBody& body)
{
	/*//dont bother if moving fastish (actually causes issues for some reason)
	//std::cout << yVelo << " - velo\n";
	if (yVelo > 0.1f || yVelo < -0.1f) {
		m_grounded = false;
		return false;
	}*/

	bool grounded = false, pA = false, pB = false;
	int numManifolds = PhysBody::GetWorld()->getDispatcher()->getNumManifolds(), index = body.GetBody()->getUserIndex();
	for (int i = 0; (i < numManifolds) && !grounded; ++i) {
		btPersistentManifold* contactManifold = PhysBody::GetWorld()->getDispatcher()->getManifoldByIndexInternal(i);
		pA = (contactManifold->getBody0()->getUserIndex() == index);
		pB = (contactManifold->getBody1()->getUserIndex() == index);

		if (pA || pB) {
			for (int j = contactManifold->getNumContacts() - 1; j >= 0; --j) {
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance() <= 0.f) {
					btVector3 normal;
					if (pB)		normal = -pt.m_normalWorldOnB;
					else		normal = pt.m_normalWorldOnB;
					//std::cout << normal.y() << " - normal\n";

					//normal threshold
					if (normal.y() > 0.4f) {
						grounded = true;
						break;
					}
				}
			}
		}
	}

	//m_grounded = grounded;
	if (grounded) {
		if (!m_grounded) {
			m_grounded = true;
			body.SetGravity(BLM::BTzero);
		}
	}
	else {
		if (m_grounded) {
			m_grounded = false;
			body.SetGravity(m_gravity);
		}
	}

	return m_grounded;
}

void Player::UseWeapon(PhysBody& body, Transform& head, float offset)
{
	if (m_currWeapon == WEAPON::FIST)
	{
		if (m_grounded)
			body.SetVelocity(BLM::BTtoGLM(
				Melee(BLM::BTtoGLM(body.GetBody()->getWorldTransform().getOrigin()) - head.GetForwards())
			));
		return;
	}

	GunProperties selectedGun = GetProperties(m_currWeapon);
	m_weaponCooldown = selectedGun.cooldown;
	LaserGun(offset, head, selectedGun.damage, selectedGun.maxRange, m_currWeapon);

	//This branch *should* not be too bad since we are comparing 2 const variables.
	if (selectedGun.type == WEAPON::SHOTGUN)
	{
		LaserGun(offset, head, selectedGun.damage, selectedGun.maxRange, m_currWeapon);
		LaserGun(offset, head, selectedGun.damage, selectedGun.maxRange, m_currWeapon);
		LaserGun(offset, head, selectedGun.damage, selectedGun.maxRange, m_currWeapon);
		LaserGun(offset, head, selectedGun.damage, selectedGun.maxRange, m_currWeapon);
	}

	//deal with ammo here
	if (--m_currWeaponAmmo <= 0) {
		SwapWeapon();
	}
}
//Not forward declared
float HalfCurve(float input) {
	return ((input - 1) * (input - 1) * (input - 1) * (input - 1) * 0.5f + 0.5f);
}

//does direction math for the shooting, ShootLazer does the actual projectile
void Player::LaserGun(float offset, Transform& head, short damage, float distance, WEAPON type)
{
	AudioEngine::Instance().GetEvent("shoot").Restart();

	glm::quat offsetQuat = glm::angleAxis(offset, glm::vec3(0.24253f, 0.97014f, 0.f));
	//shotgun range < 30.f therefore we can do spread
	if (type == WEAPON::SHOTGUN)
	{
		//shotgun spread
		offsetQuat = glm::angleAxis(glm::radians(rand() % 15 - 7.f), glm::normalize(glm::vec3(rand() % 21 - 10.f, rand() % 21 - 10.f, 0)));
	}
	glm::vec3 rayPos =
		head.GetGlobalPosition() + glm::vec3(m_gunOffset * glm::rotate(BLM::GLMMat, m_rot.y, BLM::GLMup));

	RayResult p = PhysBody::GetRaycastResult(BLM::GLMtoBT(rayPos),
		BLM::GLMtoBT(glm::rotate(offsetQuat, -head.GetForwards()) * distance * 100.f));

	if (p.hasHit()) if (p.m_closestHitFraction <= 0.01f)
	{
		if (type == WEAPON::PISTOL)
		{
			damage *= 1 - p.m_closestHitFraction * 100.f;
		}
		else if (type == WEAPON::SHOTGUN)
		{
			damage *= HalfCurve(p.m_closestHitFraction * 100.f);
		}

		glm::vec3 colour = BLM::GLMzero;

		entt::entity playerIdTest = p.m_collisionObject->getUserIndex();
		if (ECS::Exists(playerIdTest)) {
			if (ECS::HasComponent<Player>(playerIdTest)) {
				if (ECS::GetComponent<Player>(playerIdTest).TakeDamage(damage))
					++m_killCount;
				colour = glm::vec3(-2, -1, 1);
			}
		}
		Rendering::effects->ShootLaser(head.GetGlobalRotation() * offsetQuat, rayPos,
			glm::length(BLM::BTtoGLM(p.m_hitPointWorld) - rayPos), colour);
	}
	else {
		Rendering::effects->ShootLaser(head.GetGlobalRotation() * offsetQuat, rayPos, distance);
	}
}

void Player::SwapWeapon()
{
	//m_swapWeapon.play();
	AudioEngine::Instance().GetEvent("reload").Restart();

	WEAPON tempWeap = m_currWeapon;
	m_currWeapon = m_secWeapon;

	short tempAmmo = m_currWeaponAmmo;
	m_currWeaponAmmo = m_secWeaponAmmo;

	if (tempAmmo <= 0) { m_secWeapon = WEAPON::FIST; m_secWeaponAmmo = 0; }
	else { m_secWeapon = tempWeap; m_secWeaponAmmo = tempAmmo; }

	//Changing speed of player to gun speed
	GunProperties selectedGun = GetProperties(m_currWeapon);
	m_speed = selectedGun.movementSpeed;

	m_weaponCooldown += 1.f;
	if (m_weaponCooldown > 3.f)
		m_weaponCooldown = 3.f;
}

void Player::UseHeal()
{
	if (m_health == m_maxHealth)	return;

	if (m_offhand == OFFHAND::HEALPACK1) {
		m_health += 15;

		//m_swapWeapon.play();
		AudioEngine::Instance().GetEvent("reload").Restart();

		if (m_health > m_maxHealth)
			m_health = m_maxHealth;
		m_offhand = OFFHAND::EMPTY;
	}
	if (m_offhand == OFFHAND::HEALPACK2) {
		m_health += 15;

		//m_swapWeapon.play();
		AudioEngine::Instance().GetEvent("reload").Restart();

		if (m_health > m_maxHealth)
			m_health = m_maxHealth;
		m_offhand = OFFHAND::HEALPACK1;
	}
}

const Player::GunProperties& Player::GetProperties(WEAPON type) const
{
	switch (type) {
	case WEAPON::PISTOL: return pistol; break;
	case WEAPON::RIFLE: return rifle; break;
	case WEAPON::SHOTGUN: return shotgun; break;
	case WEAPON::MACHINEGUN: return machineGun; break;
	case WEAPON::CANNON: return cannon; break;
	default: return pistol; break;
	}

}

btVector3 Player::Melee(const glm::vec3& pos)
{
	m_punched = true;

	bool hit = false;

	ECS::GetRegistry()->view<Player, PhysBody>().each(
		[&](Player& p, PhysBody& body) {
		//if (p.m_user != m_user) if (body.TestAABB(pos, 0.5f)) {
		if (p.m_camPos != m_camPos) if (body.TestAABB(pos, 0.5f)) {
			if (p.TakeDamage(m_meleeDmg))
				++m_killCount;
			hit = true;
		}
	}
	);

	//if (hit)	m_punch.play();
	//else		m_wiff.play();
	AudioEvent& evnt = AudioEngine::Instance().GetEvent("hit");
	evnt.SetParameter("punchHit", hit);
	evnt.Restart();

	return BLM::BTzero;
}

bool Player::PickUpWeapon(WEAPON pickup)
{
	GunProperties selectedGun = GetProperties(pickup);
	if (m_currWeapon == WEAPON::FIST) {
		m_currWeapon = pickup;
		m_currWeaponAmmo = selectedGun.ammoCapacity;
		m_speed = selectedGun.movementSpeed;
		//m_swapWeapon.play();
		AudioEngine::Instance().GetEvent("pickup").Restart();

		return true;
	}
	if (m_secWeapon == WEAPON::FIST) {
		if (m_currWeapon == pickup)
			return false;
		m_secWeapon = pickup;
		m_secWeaponAmmo = selectedGun.ammoCapacity;
		m_speed = selectedGun.movementSpeed;

		AudioEngine::Instance().GetEvent("pickup").Restart();

		return true;
	}
	return false;
}

bool Player::PickUpOffhand(OFFHAND pickup)
{
	if (m_offhand == OFFHAND::EMPTY || m_offhand == OFFHAND::HEALPACK1) {
		m_offhand = pickup;

		AudioEngine::Instance().GetEvent("pickup").Restart();

		return true;
	}
	return false;
}

ObjLoader& Player::GetWeaponModel(WEAPON choice)
{
	switch (choice) {
		//draw gun
	case WEAPON::CANNON:		return m_canon;
	case WEAPON::RIFLE:			return m_rifle;
	case WEAPON::MACHINEGUN:	return m_machineGun;
	case WEAPON::SHOTGUN:		return m_shotgun;
	case WEAPON::SWORD:			return m_sword;

		//case WEAPON::PISTOL:		return m_pistol;
			//WEAPON::FIST is included
	default:	return m_pistol;
	}
}

ObjMorphLoader& Player::GetOffhandModel(OFFHAND choice)
{
	return m_healPack;

	//do for real
	/*switch (choice) {
	case OFFHAND::HEALPACK1:	return m_healPack;
	case OFFHAND::HEALPACK2:	return m_healPack;
	default:	return m_healPack;
	}*/
}

Sprite& Player::GetIcon(WEAPON choice)
{
	switch (choice) {
		//fist will be here
	default:					return m_fistIcon;
	case WEAPON::PISTOL:		return m_pistolIcon;
	case WEAPON::RIFLE:			return m_rifleIcon;
	case WEAPON::CANNON:		return m_canonIcon;
	case WEAPON::MACHINEGUN:	return m_machineGunIcon;
	case WEAPON::SHOTGUN:		return m_shotgunIcon;
	}
}
