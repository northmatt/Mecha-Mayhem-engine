#include "Player.h"
#include "Utilities/Time.h"
#include "Utilities/BLM.h"
#include "ECS.h"

float Player::m_camDistance = 5.f;
float Player::m_dashDistance = 7.5f;
Camera Player::m_orthoCam = Camera();
const glm::mat4 Player::m_modelOffset = glm::mat4(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, -1, 0, 0
);
Sound2D Player::shootLaser = Sound2D();

void Player::Init(int width, int height)
{
	m_orthoCam.SetOrthoHeight(10).SetPosition(glm::vec3(0, 0, 0)).SetNear(-10).Setfar(10)
		.SetIsOrtho(true).ResizeWindow(width, height);
	shootLaser = Sound2D("laser.mp3", "shooting");

	ObjMorphLoader("char/idle", true).LoadMeshs("char/walk", true)
		.LoadMeshs("char/air", true).LoadMeshs("char/death", true);
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
		float healthPercent = float(m_health) / m_maxHealth;
		float dashPercent = float(m_dashTimer) / m_dashDelay;

		m_healthBar.SetWidth(14.95f * healthPercent);
		m_dashBar.SetWidth(9.15f * (1 - dashPercent));
		m_healthBar.Draw(m_orthoCam.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			7.475f - healthPercent * 7.475f, -8.5f, -7, 1
		));
		m_healthBarBack.Draw(m_orthoCam.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, -8.5f, -6.5f, 1
		));
		m_dashBar.Draw(m_orthoCam.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			dashPercent * 4.575f, -7.25f, -7, 1
		));
		m_dashBarBack.Draw(m_orthoCam.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, -7.25f, -6.5f, 1
		));

		//drawn last cause layering
		m_healthBarOutline.Draw(m_orthoCam.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, -8.5f, -7.5f, 1
		));
		m_dashBarOutline.Draw(m_orthoCam.GetViewProjection(), glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, -7.25f, -7.5f, 1
		));

		//is false when cam is too close
		if (!m_drawSelf)	return;
	}
	m_charModel.Draw(model + m_modelOffset);
}

void Player::Update(PhysBody& body)
{
	m_charModel.Update(Time::dt);
	updateDash();
	//when dead
	if (m_respawnTimer > 0) {
		m_respawnTimer -= Time::dt;
		if (m_respawnTimer <= 0) {
			m_respawnTimer = 0;

			//reset stats
			m_health = m_maxHealth;
			m_dashTimer = 0.f;
			m_weaponCooldown = 0.f;
			m_currWeapon = WEAPON::FIST;
			m_offhand = OFFHAND::EMPTY;
			body.SetPosition(m_spawnPos);
			body.SetGravity(btVector3(0, -100, 0));
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
		body.SetGravity(btVector3(0, 0, 0));
		body.SetPosition(m_spawnPos + glm::vec3(0, 50, 0));
		head.SetRotation(glm::quat(-0.71f, 0.71f, 0.f, 0.f));
		return;
	}
	if (m_punched) {
		m_charModel.BlendTo(m_charModelIndex + "/death");

		if (m_charModel.IsDone())	m_punched = false;
		else						return;
	}

	//controls
	{
		m_rot.x += ControllerInput::GetRY(m_user) * 3.f * Time::dt;
		m_rot.y += ControllerInput::GetRX(m_user) * 3.f * Time::dt;

		if (m_rot.x > pi)			m_rot.x = pi;
		else if (m_rot.x < -pi)		m_rot.x = -pi;

		body.SetRotation(glm::rotate(m_startRot, m_rot.y, glm::vec3(0, -1, 0)));
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
				//punch
				vel.x = 0;
				vel.z = 0;
			}
			else {
				//ground anims
				if (vel.x != 0 || vel.z != 0)
					m_charModel.BlendTo(m_charModelIndex + "/walk");
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

		//only do dash check if not moving
		if (!m_punched) {
			//dash if moving
			if (vel.x != 0 || vel.z != 0) {
				glm::vec3 normalized = glm::normalize(glm::vec3(vel.x, 0, vel.z));
				float percent = std::max(fabsf(vel.x), fabsf(vel.z));
				vel.x = normalized.x * m_speed * percent;
				vel.z = normalized.z * m_speed * percent;

				vel = glm::vec4(vel, 1) * glm::rotate(glm::mat4(1.f), m_rot.y, glm::vec3(0, 1, 0));

				if (m_dashTimer == 0) {
					if (ControllerInput::GetLTDown(m_user)) {
						m_dashTimer = m_dashDelay;
						glm::vec3 ogPos = BLM::BTtoGLM(body.GetTransform().getOrigin());
						btVector3 pos = PhysBody::GetRaycastWithDistanceLimit(
							ogPos, glm::vec3(vel.x, 0, vel.z) * m_dashDistance * 10.f, m_dashDistance);

						ShootDash((pos - body.GetTransform().getOrigin()).length(), ogPos,
							glm::rotation(glm::vec3(normalized.x, 0, -normalized.z), glm::vec3(0, 0, 1)));
						body.SetPosition(pos);
					}
				}
			}
			//dash without moving
			else if (m_dashTimer == 0) {
				if (ControllerInput::GetLTDown(m_user)) {
					m_dashTimer = m_dashDelay;
					glm::vec3 ogPos = BLM::BTtoGLM(body.GetTransform().getOrigin());
					btVector3 pos = PhysBody::GetRaycastWithDistanceLimit(ogPos, glm::vec4(0, 0, m_dashDistance * -10.f, 1)
						* glm::rotate(glm::mat4(1.f), m_rot.y, glm::vec3(0, 1, 0)), m_dashDistance);

					ShootDash((pos - body.GetTransform().getOrigin()).length(), ogPos,
						BLM::BTtoGLM(body.GetTransform().getRotation()));
					body.SetPosition(pos);
				}
			}
		}
		body.SetVelocity(vel);
	}

	//gun
	{
		if (ControllerInput::GetButtonDown(BUTTON::Y, m_user) && m_secWeapon != WEAPON::FIST) {
			WEAPON temp = m_currWeapon;
			m_currWeapon = m_secWeapon;
			m_secWeapon = temp;
		}
		if (m_weaponCooldown == 0) {
			if (ControllerInput::GetRTRaw(m_user)) {
				UseWeapon(body);
				shootLaser.play();
				m_weaponCooldown = 0.5f;
			}
		}
	}

	//camera
	{
		glm::vec3 rayPos = head.ComputeScalessGlobal().GetGlobalPosition();
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
}

void Player::UseWeapon(PhysBody& body)
{
	switch (m_currWeapon) {
	case WEAPON::FIST:
		m_punched = true;
		body.SetVelocity(btVector3(0, 0, 0));
		break;
	case WEAPON::GUN:
		//shoot
		m_weaponCooldown = 0.5f;
		break;
	default:	break;
	}
}

void Player::PickUpWeapon(WEAPON pickup)
{
	if (m_currWeapon != WEAPON::FIST)
		if (m_secWeapon != WEAPON::FIST)
			m_secWeapon = pickup;
}

void Player::PickUpOffhand(OFFHAND pickup)
{
	if (m_offhand != OFFHAND::EMPTY)
		m_offhand = pickup;
}

void Player::ShootDash(float width, glm::vec3 pos, glm::quat rotation)
{
	if (Dash == entt::null) {
		Dash = ECS::CreateEntity();
		ECS::AttachComponent<ObjMorphLoader>(Dash).LoadMeshs("effects/dash", true);
		ECS::GetComponent<Transform>(Dash).SetRotation(rotation).SetPosition(pos).SetScale(glm::vec3(1, 1, width));
	}
}

void Player::updateDash()
{
	if (Dash != entt::null) {
		if (ECS::GetComponent<ObjMorphLoader>(Dash).IsDone()) {
			ECS::DestroyEntity(Dash);
			Dash = entt::null;
		}
	}
}
