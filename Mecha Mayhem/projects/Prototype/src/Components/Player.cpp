#include "Player.h"
#include "Utilities/Time.h"
#include "Utilities/Rendering.h"
#include "ECS.h"

#include "Utilities/Input.h"

const glm::mat4 Player::m_gunOffset = glm::mat4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0.3f, 0.1f, -0.3f, 1
);
const glm::mat4 Player::m_modelOffset = glm::mat4(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, -1, 0, 0
);

float Player::m_camDistance = 5.f;
float Player::m_dashDistance = 7.5f;
glm::vec3 Player::m_skyPos = glm::vec3(0, 50, 0);

Camera Player::m_orthoCam = {};
ObjLoader Player::m_pistol = {};
ObjLoader Player::m_canon = {};
ObjLoader Player::m_rifle = {};
ObjLoader Player::m_machineGun = {};
ObjLoader Player::m_shotgun = {};
ObjLoader Player::m_sword = {};
Sound2D Player::m_shootLaser = {};
Sound2D Player::m_hitSound = {};
Sound2D Player::m_deathSound = {};
Sound2D Player::m_swapWeapon = {};
Sound2D Player::m_walk[5] = {};
Sound2D Player::m_wiff = {};
Sound2D Player::m_punch = {};
Sprite Player::m_healthBarOutline = {};
Sprite Player::m_healthBar = {};
Sprite Player::m_healthBarBack = {};
Sprite Player::m_dashBarOutline = {};
Sprite Player::m_dashBar = {};
Sprite Player::m_dashBarBack = {};
Sprite Player::m_reticle = {};

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

	m_shootLaser = { "laser.mp3", "shooting" };
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
	m_punch = { "MetalFloor/3StepNoise.mp3", "sfx" };

	m_healthBarOutline = { "healthbar.png", 15.96f, 1.5f };
	m_healthBar = { glm::vec4(0, 0, 1, 1.f), 14.95f, 0.9f };
	m_healthBarBack = { glm::vec4(0, 0, 0, 1.f), 14.95f, 0.9f };

	m_dashBarOutline = { "energybar.png", 10.38, 1.5f };
	m_dashBar = { glm::vec4(1, 1, 1, 1.f), 9.15f, 0.5f };
	m_dashBarBack = { glm::vec4(0, 0, 0, 1.f), 9.15f, 0.5f };

	m_reticle = { "reticle.png", 1.f, 1.f };

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

void Player::SetUIAspect(int width, int height)
{
	m_orthoCam.ResizeWindow(width, height);
}

Player& Player::Init(CONUSER user, int characterModel)
{
	m_user = user;
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

void Player::Draw(const glm::mat4& model, short camNum, short numOfCams)
{
	if (short(m_user) == camNum) {
		//draw ui
		float healthPercent = float(m_health) / m_maxHealth;
		float dashPercent = float(m_dashTimer) / m_dashDelay;

		m_healthBar.SetWidth(14.95f * healthPercent);
		m_dashBar.SetWidth(9.15f * (1 - dashPercent));

		glm::mat4 VP = m_orthoCam.GetViewProjection();

		m_healthBar.Draw(VP, glm::mat4(
			1, 0, 0, 0,			0, 1, 0, 0,			0, 0, 1, 0,			(1 - healthPercent) * 7.475f, -8.5f, -9.9f, 1		));
		m_healthBarBack.Draw(VP, glm::mat4(
			1, 0, 0, 0,			0, 1, 0, 0,			0, 0, 1, 0,			0, -8.5f, -9.8f, 1		));
		m_dashBar.Draw(VP, glm::mat4(
			1, 0, 0, 0,			0, 1, 0, 0,			0, 0, 1, 0,			dashPercent * 4.575f, -7.25f, -9.9f, 1		));
		m_dashBarBack.Draw(VP, glm::mat4(
			1, 0, 0, 0,			0, 1, 0, 0,			0, 0, 1, 0,			0, -7.25f, -9.8f, 1		));

		//drawn last cause layering
		m_healthBarOutline.Draw(VP, glm::mat4(
			1, 0, 0, 0,			0, 1, 0, 0,			0, 0, 1, 0,			0, -8.5f, -10.f, 1		));
		m_dashBarOutline.Draw(VP, glm::mat4(
			1, 0, 0, 0,			0, 1, 0, 0,			0, 0, 1, 0,			0, -7.25f, -10.f, 1		));

		m_reticle.Draw(VP, glm::mat4(
			1, 0, 0, 0,			0, 1, 0, 0,			0, 0, 1, 0,			0, 0, -10, 1		));

		//is false when cam is too close
		if (!m_drawSelf)	return;
	}
	if (!m_punched) {
		switch (m_currWeapon) {
			//draw gun
		case WEAPON::PISTOL:		m_pistol.Draw(model * m_gunOffset);		break;
		case WEAPON::CANON:			m_canon.Draw(model * m_gunOffset);		break;
		case WEAPON::RIFLE:			m_rifle.Draw(model * m_gunOffset);		break;
		case WEAPON::MACHINEGUN:	m_machineGun.Draw(model * m_gunOffset);	break;
		case WEAPON::SHOTGUN:		m_shotgun.Draw(model * m_gunOffset);	break;
		case WEAPON::SWORD:			m_sword.Draw(model * m_gunOffset);	break;

			//WEAPON::FIST is included
		default:	break;
		}
	}
	m_charModel.Draw(model + m_modelOffset);
}

void Player::Update(PhysBody& body)
{
	m_charModel.Update(Time::dt);
	//when dead
	if (m_respawnTimer > 0) {
		body.SetGravity(BLM::BTzero);
		m_charModel.BlendTo(m_charModelIndex + "/death", 0.25f);
		if (m_respawnTimer == m_respawnDelay)
			m_deathPos = BLM::BTtoGLM(body.GetTransform().getOrigin());
		m_respawnTimer -= Time::dt;
		if (m_respawnTimer <= 0) {
			m_charModel.BlendTo(m_charModelIndex + "/idle");
			m_respawnTimer = 0;

			//reset stats
			m_health = m_maxHealth;
			m_dashTimer = 0.f;
			m_weaponCooldown = 0.f;
			body.SetPosition(m_spawnPos);
			body.SetGravity(btVector3(0, -100, 0));
			body.SetAwake();
		}
		else {
			float percent = m_respawnTimer / m_respawnDelay;
			body.SetPosition((1 - percent) * m_skyPos + percent * m_deathPos);
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
}

void Player::GetInput(PhysBody& body, Transform& head, Transform& personalCam)
{
	//dont care if not a player or dead
	if (m_user == CONUSER::NONE)	return;
	if (m_health == 0) {
		body.SetVelocity(BLM::BTzero);
		head.SetRotation(glm::quat(-0.71f, 0.71f, 0.f, 0.f));
		return;
	}

	if (m_punched) {
		m_charModel.BlendTo(m_charModelIndex + "/punch");

		if (m_charModel.IsDone())	m_punched = false;
		else						return;
	}

	//controls
	{
		m_rot.x += ControllerInput::GetRY(m_user) * 3.f * Time::dt;
		m_rot.y += ControllerInput::GetRX(m_user) * 3.f * Time::dt;

		if (m_rot.x > pi)			m_rot.x = pi;
		else if (m_rot.x < -pi)		m_rot.x = -pi;

		body.SetRotation(glm::rotate(m_startRot, -m_rot.y, BLM::GLMup));
		head.SetRotation(glm::rotate(m_startRot, m_rot.x, glm::vec3(1, 0, 0)));

		glm::vec3 vel = glm::vec3(0.f);
		vel.y = body.SetAwake().GetVelocity().y();

		vel.x += ControllerInput::GetLX(m_user);
		vel.z -= ControllerInput::GetLY(m_user);

		if (m_grounded = vel.y < 0.1f && vel.y > -0.1f) {
			//jump
			if (ControllerInput::GetButtonDown(BUTTON::A, m_user)) {
			//if (ControllerInput::GetButton(BUTTON::A, m_user)) {
				vel.y = 20.f;
				m_jumpHeld = Time::dt;
			}
			else m_jumpHeld = 0;

			if (m_punched = ControllerInput::GetButtonDown(BUTTON::X, m_user)) {
				//punch only when on ground
				vel = BLM::BTtoGLM(Melee(BLM::BTtoGLM(body.GetBody()->getWorldTransform().getOrigin())
					- head.GetForwards()));
			}
			else {
				//ground anims
				if (vel.x != 0 || vel.z != 0) {
					m_charModel.SetSpeed(std::max(fabsf(vel.x), fabsf(vel.z))).
						BlendTo(m_charModelIndex + "/walk");
					if (m_charModel.Getp0() == 0 || m_charModel.Getp0() == 4) {
						if (!m_stepped) {
							m_walk[rand() % 5].play();
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
			if (m_jumpHeld > 0 && m_jumpHeld < 0.1f) {
				if (ControllerInput::GetButton(BUTTON::A, m_user)) {
					vel.y = 20.f;
					m_jumpHeld += Time::dt;
				}
				else m_jumpHeld = 0;
			}
			m_charModel.BlendTo(m_charModelIndex + "/air");
		}

		//only do other input check if not moving
		if (!m_punched) {
			if (ControllerInput::GetButtonDown(BUTTON::B, m_user)) {
				UseHeal();
				TakeDamage(100);
			}

			//dash if moving
			if (vel.x != 0 || vel.z != 0) {
				glm::vec3 normalized = glm::normalize(glm::vec3(vel.x, 0, vel.z));
				float percent = std::max(fabsf(vel.x), fabsf(vel.z));
				vel.x = normalized.x * m_speed * percent;
				vel.z = normalized.z * m_speed * percent;

				vel = glm::vec4(vel, 1) * glm::rotate(glm::mat4(1.f), m_rot.y, BLM::GLMup);

				if (m_dashTimer == 0) {
					//if (ControllerInput::GetLTDown(m_user)) {
					if (ControllerInput::GetLTRaw(m_user)) {
						m_dashTimer = m_dashDelay;
						glm::vec3 ogPos = BLM::BTtoGLM(body.GetTransform().getOrigin());
						btVector3 pos = PhysBody::GetRaycastWithDistanceLimit(
							ogPos, glm::vec3(vel.x, 0, vel.z) * m_dashDistance * 10.f, m_dashDistance);
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
				//if (ControllerInput::GetLTDown(m_user)) {
				if (ControllerInput::GetLTRaw(m_user)) {
					m_dashTimer = m_dashDelay;
					glm::vec3 ogPos = BLM::BTtoGLM(body.GetTransform().getOrigin());
					btVector3 pos = PhysBody::GetRaycastWithDistanceLimit(ogPos,
						glm::vec4(0, 0, m_dashDistance * -50.f, 1)
						* glm::rotate(glm::mat4(1.f), m_rot.y, BLM::GLMup), m_dashDistance);
					glm::vec3 newPos = BLM::BTtoGLM(pos);
					Rendering::effects->ShootDash(BLM::BTtoGLM(body.GetTransform().getRotation()),
						newPos, glm::length(newPos - ogPos));
					body.SetPosition(pos);
				}
			}
		}
		body.SetVelocity(vel);
	}

	float rayOff = 0;

	//camera
	{
		glm::vec3 rayPos = head.GetGlobalPosition();
		btVector3 test = PhysBody::GetRaycast(rayPos, head.GetForwards() * (m_camDistance * 50.f));
		if (m_rot.x < pi * 0.25f) {
			if (test != btVector3()) {
				float distance = glm::length(rayPos - BLM::BTtoGLM(test));

				if (distance > m_camDistance) {
					personalCam.SetPosition(glm::vec3(0.4f, 0, m_camDistance));
					rayOff = -0.0175f;
				}
				else {
					personalCam.SetPosition(glm::vec3(0.4f, 0, distance));
					rayOff = -0.0175f * (distance / m_camDistance);
				}
				m_drawSelf = (distance > 0.75f);
			}
			else {
				personalCam.SetPosition(glm::vec3(0.4f, 0, m_camDistance));
				rayOff = -0.0175f;
				m_drawSelf = true;
			}
		}
		else if (m_rot.x < pi * 0.5f) {
			float t = (m_rot.x / pi - 0.25f) * 4;
			rayOff = -0.0175f * (1 - t);
			personalCam.SetPosition(glm::vec3(0.4f * (1 - t), 0, (1 - t) * m_camDistance - t * 0.5f));
			m_drawSelf = t < 0.5f;
		}
		else {
			personalCam.SetPosition(glm::vec3(0, 0, 0.5f));
			m_drawSelf = false;
		}
	}

	//gun
	{
		if (ControllerInput::GetButtonDown(BUTTON::Y, m_user) && m_secWeapon != WEAPON::FIST) {
			m_swapWeapon.play();
			WEAPON tempWeap = m_currWeapon;
			m_currWeapon = m_secWeapon;
			m_secWeapon = tempWeap;

			short tempAmmo = m_currWeaponAmmo;
			m_currWeaponAmmo = m_secWeaponAmmo;
			m_secWeaponAmmo = tempAmmo;
		}
		if (m_weaponCooldown == 0) {
			if (ControllerInput::GetRTRaw(m_user)) {
				UseWeapon(body, head, rayOff);
			}
		}
	}
}

void Player::UseWeapon(PhysBody& body, Transform& head, float offset)
{
	switch (m_currWeapon) {
	case WEAPON::FIST:
		body.SetVelocity(BLM::BTtoGLM(Melee(BLM::BTtoGLM(body.GetBody()->getWorldTransform().getOrigin())
				- head.GetForwards())));
		break;
	default:	//break;	for demo, all guns do the same
	//case WEAPON::PISTOL:
		//shoot
		{
			short damage = 1;
			m_weaponCooldown = 0.25f;
			m_shootLaser.play();

			glm::quat offsetQuat = glm::angleAxis(offset, BLM::GLMup);

			glm::vec3 rayPos = head.GetGlobalPosition();
			glm::vec3 forwards = glm::rotate(offsetQuat, -head.GetForwards());
			RayResult p = PhysBody::GetRaycastResult(BLM::GLMtoBT(rayPos),
				BLM::GLMtoBT(forwards * 2000.f));
			if (p.hasHit())
			{
				entt::entity playerIdTest = p.m_collisionObject->getUserIndex();
				if (ECS::Exists(playerIdTest)) {
					if (ECS::HasComponent<Player>(playerIdTest)) {
						if (ECS::GetComponent<Player>(playerIdTest).TakeDamage(damage))
							++m_killCount;
					}
				}
				Rendering::effects->ShootLaser(head.GetGlobalRotation() * offsetQuat, rayPos,
					glm::length(BLM::BTtoGLM(p.m_hitPointWorld) - rayPos));
			}
			else {
				Rendering::effects->ShootLaser(head.GetGlobalRotation() * offsetQuat, rayPos, 2000.f);
			}

			//deal with ammo here
			if (--m_currWeaponAmmo <= 0) {
				m_currWeapon = m_secWeapon;
				m_secWeapon = WEAPON::FIST;
			}
		}
		break;
	}
}

void Player::UseHeal()
{
	if (m_offhand == OFFHAND::HEALPACK1) {
		m_health += 3;
		if (m_health > m_maxHealth)
			m_health = m_maxHealth;
		m_offhand = OFFHAND::EMPTY;
	}
	if (m_offhand == OFFHAND::HEALPACK2) {
		m_health += 3;
		if (m_health > m_maxHealth)
			m_health = m_maxHealth;
		m_offhand = OFFHAND::HEALPACK1;
	}
}

btVector3 Player::Melee(const glm::vec3& pos)
{
	m_punched = true;

	bool hit = false;

	ECS::GetRegistry()->view<Player, PhysBody>().each(
		[&](Player& p, PhysBody& body) {
			if (p.m_user != m_user) {
				if (body.TestAABB(pos, 0.5f)) {
					p.TakeDamage(1);
				}
			}
		}
	);

	if (hit)	m_punch.play();
	else		m_wiff.play();

	return BLM::BTzero;
}

bool Player::PickUpWeapon(WEAPON pickup)
{
	if (m_currWeapon == WEAPON::FIST) {
		m_currWeapon = pickup;
		//based on weapon, add ammo
		m_currWeaponAmmo = 20;
		if (pickup == WEAPON::MACHINEGUN)
			m_currWeaponAmmo = 100;
		m_swapWeapon.play();
		return true;
	}
	if (m_secWeapon == WEAPON::FIST) {
		m_secWeapon = pickup;
		//based on weapon, add ammo
		m_secWeaponAmmo = 20;
		if (pickup == WEAPON::MACHINEGUN)
			m_secWeaponAmmo = 100;

		m_swapWeapon.play();
		return true;
	}
	return false;
}

bool Player::PickUpOffhand(OFFHAND pickup)
{
	if (m_offhand == OFFHAND::EMPTY) {
		m_offhand = pickup;
		return true;
	}
	return false;
}
