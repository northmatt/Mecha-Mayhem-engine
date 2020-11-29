#include "Player.h"
#include "Utilities/Time.h"
#include "Utilities/BLM.h"

float Player::m_camDistance = 5.f;
Camera Player::m_orthoCam = Camera();
const glm::mat4 Player::m_modelOffset = glm::mat4(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, -1, 0, 0
);

void Player::Init(int width, int height)
{
	m_orthoCam.SetOrthoHeight(10).SetPosition(glm::vec3(0, 0, 0)).SetNear(-10).Setfar(10)
		.SetIsOrtho(true).ResizeWindow(width, height);

	ObjMorphLoader("char/idle", true).LoadMeshs("char/walk", true)
		.LoadMeshs("char/air", true).LoadMeshs("char/death", true);
}

void Player::SetUIAspect(int width, int height)
{
	m_orthoCam.ResizeWindow(width, height);
}

Player& Player::Init(CONUSER user, short characterModel)
{
	m_user = user;
	switch (characterModel) {
		//dummy (case 0 is also here)
	default:	m_charModelIndex = "char";	m_charModel.LoadMeshs("char/idle", true);	break;
		//JJ's
	case 1:		m_charModelIndex = "char";	m_charModel.LoadMeshs("char/idle", true);	break;
		//JL's
	case 2:		m_charModelIndex = "char2";	m_charModel.LoadMeshs("char2/idle", true);	break;
	}

	return *this;
}

void Player::Draw(const glm::mat4& model, short camNum, short numOfCams)
{
	if (short(m_user) == camNum) {
		//draw ui
		m_healthBar.Draw(m_orthoCam.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, -8.5f, 0, 1
		));
		m_dashBar.Draw(m_orthoCam.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, -7.25f, 0, 1
		));

		//is false when cam is too close
		if (!m_drawSelf)	return;
	}
	m_charModel.Draw(model + m_modelOffset);
}

void Player::Update(PhysBody& body)
{
	m_charModel.Update(Time::dt);

	//when dead
	if (m_respawnTimer > 0) {
		m_respawnTimer -= Time::dt;
		if (m_respawnTimer <= 0) {
			m_respawnTimer = 0;

			//reset stats
			m_health = m_maxHealth;
			m_dashTimer = m_dashDelay;
			m_currWeapon = WEAPON::FIST;
			m_offhand = OFFHAND::EMPTY;
			body.SetPosition(m_spawnPos);
		}
		return;
	}
}

void Player::GetInput(PhysBody& body, Transform& head, Transform& personalCam)
{
	//dont care if not a player or dead
	if (m_user == CONUSER::NONE || m_health == 0)	return;

	if (ControllerInput::GetButton(BUTTON::X, m_user)) {
		m_charModel.BlendTo(m_charModelIndex + "/death", 0.25f);
		return;
	}
	body.SetAwake();

	m_rot.x += ControllerInput::GetRY(m_user) * 3.f * Time::dt;
	m_rot.y += ControllerInput::GetRX(m_user) * 3.f * Time::dt;

	if (m_rot.x > pi)			m_rot.x = pi;
	else if (m_rot.x < -pi)		m_rot.x = -pi;

	body.SetRotation(glm::rotate(m_startRot, m_rot.y, glm::vec3(0, -1, 0)));
	head.SetRotation(glm::rotate(m_startRot, m_rot.x, glm::vec3(1, 0, 0))).ComputeGlobal();

	glm::vec3 vel = glm::vec3(0.f);
	vel.y = body.GetVelocity().y();

	vel.x += ControllerInput::GetLX(m_user) * 15;
	vel.z -= ControllerInput::GetLY(m_user) * 15;

	if (grounded = vel.y < 0.1f && vel.y > -0.1f) {
		//jump
		if (ControllerInput::GetButtonDown(BUTTON::A, m_user))
			vel.y = 20;		//low jump

		//ground anims
		if (vel.x != 0 || vel.z != 0)
			m_charModel.BlendTo(m_charModelIndex + "/walk");
		else
			m_charModel.BlendTo(m_charModelIndex + "/idle");
	}
	else {
		m_charModel.BlendTo(m_charModelIndex + "/air");
	}

	vel = glm::vec4(vel, 1) * glm::rotate(glm::mat4(1.f), m_rot.y, glm::vec3(0, 1, 0));
	body.SetVelocity(vel);

	glm::vec3 rayPos = head.ComputeScalessGlobal().GetGlobalPosition();
	//camera
	btVector3 test = PhysBody::GetRaycast(rayPos, head.GetForwards() * (m_camDistance * 10.f));
	if (m_rot.x < pi * 0.25f) {
		if (test != btVector3()) {
			float distance = glm::length(rayPos - BLM::BTtoGLM(test));

			if (distance > m_camDistance)
				personalCam.SetPosition(glm::vec3(0, 0, m_camDistance));
			else
				personalCam.SetPosition(glm::vec3(0, 0, distance));
			m_drawSelf = distance > 0.75f;
		}
		else {
			personalCam.SetPosition(glm::vec3(0, 0, m_camDistance));
			m_drawSelf = true;
		}
	}
	else if (m_rot.x < pi * 0.5f) {
		float t = (m_rot.x / pi - 0.25f) * 4;
		personalCam.SetPosition(glm::vec3(0, 0, (1 - t) * m_camDistance - t * 0.5f));
		m_drawSelf = t < 0.5f;
	}
	else {
		personalCam.SetPosition(glm::vec3(0, 0, 0.5f));
		m_drawSelf = false;
	}
}
