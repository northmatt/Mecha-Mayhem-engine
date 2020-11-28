#include "Player.h"
#include "Utilities/Time.h"
#include "Utilities/BLM.h"

const float Player::m_camDistance = 5.f;
Camera Player::m_orthoCam = Camera();
const glm::mat4 Player::m_modelOffset = glm::mat4(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, -1, 0, 0
);

void Player::Init(float aspectRatio)
{
	m_orthoCam.SetOrthoHeight(10).SetPosition(glm::vec3(0, 0, 0))
		.SetIsOrtho(true).ResizeWindow(aspectRatio, 1.f);

	ObjMorphLoader("char/idle", true).LoadMeshs("char/walk", true).LoadMeshs("char/air", true);
}

void Player::SetUIAspect(float aspectRatio)
{
	m_orthoCam.ResizeWindow(aspectRatio, 1.f);
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

void Player::Draw(const glm::mat4& model, short camNum)
{
	if (short(m_user) == camNum) {
		//draw ui

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

	{
		m_rot.x += ControllerInput::GetRY(m_user) * 3.f * Time::dt;
		m_rot.y += ControllerInput::GetRX(m_user) * 3.f * Time::dt;

		if (m_rot.x > pi)			m_rot.x = pi;
		else if (m_rot.x < -pi)		m_rot.x = -pi;

		{
			//glm::quat rotf = glm::rotate(startQuat, rot.y, glm::vec3(0, -1, 0));
			//rotf = glm::rotate(rotf, rot.x, glm::vec3(1, 0, 0));
			body.SetRotation(glm::rotate(m_startRot, m_rot.y, glm::vec3(0, -1, 0)));
			head.SetRotation(glm::rotate(m_startRot, m_rot.x, glm::vec3(1, 0, 0))).ComputeGlobal();
		}
		/*glm::mat4 rotf = glm::rotate(glm::mat4(1.f), rot.x, glm::vec3(1, 0, 0));
		rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
		camTrans.SetRotation(glm::mat3(rotf));*/

		glm::vec3 vel = glm::vec3(0.f);
		vel.y = body.GetVelocity().y();

		vel.x += ControllerInput::GetLX(m_user) * 15;
		vel.z -= ControllerInput::GetLY(m_user) * 15;

		if (ControllerInput::GetButton(BUTTON::B, m_user) && vel.y > -500) {
			vel.y -= 15 * Time::dt;
			//ballPhys.SetVelocity(btVector3(0.f, 0.f, 0.f));
		}
		if (ControllerInput::GetButtonDown(BUTTON::A, m_user)) {
			vel.y = 35 /*x at least two times this*/;
			//pos.y += 15;
		}

		grounded = vel.y > 0.1f || vel.y < -0.1f;

		if (!grounded) {
			m_charModel.BlendTo(m_charModelIndex + "/air");
		}
		else if (vel.x != 0 || vel.z != 0) {
			m_charModel.BlendTo(m_charModelIndex + "/walk");
		}
		else {
			m_charModel.BlendTo(m_charModelIndex + "/idle");
		}
		vel = glm::vec4(vel, 1) * glm::rotate(glm::mat4(1.f), m_rot.y, glm::vec3(0, 1, 0));
		body.SetVelocity(vel);
	}

	glm::vec3 rayPos = head.ComputeScalessGlobal().GetGlobalPosition();
	//camera
	RayResult test = PhysBody::GetRaycastResult(
		BLM::GLMtoBT(rayPos), BLM::GLMtoBT(head.GetForwards() * m_camDistance * 10.f)
	);
	if (m_rot.x < pi * 0.25f) {
		if (test.hasHit()) {
			personalCam.SetPosition(glm::vec3(0, 0, (test.m_closestHitFraction >= 0.1f ?
				m_camDistance : test.m_closestHitFraction * m_camDistance * 10.f - 0.5f))
			);
			m_drawSelf = test.m_closestHitFraction > 0.033f;
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
