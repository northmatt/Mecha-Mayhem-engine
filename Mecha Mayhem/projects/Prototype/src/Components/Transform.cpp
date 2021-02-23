#include "Transform.h"
#include "ECS.h"

glm::mat4 one = glm::mat4(1.f);

Transform::Transform()
{
	m_dirty = true;
}

Transform::Transform(const glm::vec3& pos)
{
	m_position = pos;
	m_dirty = true;
}

Transform::Transform(const glm::vec3& pos, const glm::quat& rot)
{
	m_position = pos;
	m_rotation = rot;
	m_dirty = true;
}

Transform::Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
{
	m_position = pos;
	m_rotation = rot;
	m_scale = scale;
	m_dirty = true;
}

Transform& Transform::ChildTo(entt::entity index)
{
	//check if it exists
	if (ECS::GetRegistry()->valid(index)) {
		m_parent = index;
	}

	return *this;
}

Transform& Transform::UnChild()
{
	m_position = GetGlobalPosition();
	m_rotation = GetGlobalRotation();
	
	m_parent = entt::null;
	m_dirty = true;

	return *this;
}

Transform& Transform::SetUsingParentScale(bool yes)
{
	m_usingParentScale = yes;
	m_dirty = true;

	return *this;
}

Transform& Transform::UseAsParent(const Transform& other)
{
	m_global = other.m_global * GetModel();

	m_position = GetGlobalPosition();
	m_rotation = GetGlobalRotation();

	return *this;
}

Transform& Transform::UseAsParent(const glm::mat4& model)
{
	m_global = model * GetModel();

	m_position = GetGlobalPosition();
	m_rotation = GetGlobalRotation();

	return *this;
}

Transform& Transform::ComputeGlobal()
{
	//parent stuff
	if (m_parent != entt::null) {
		//check if the parent exists, just in case
		if (ECS::GetRegistry()->valid(m_parent)) {
			if (m_usingParentScale)
				m_global = ECS::GetComponent<Transform>(m_parent).GetModel();
			else
				m_global = ECS::GetComponent<Transform>(m_parent).GetScalessModel();
		}
		else	UnChild();

	}
	else	m_global = one;

	m_global = glm::translate(m_global, m_position);
	m_global *= glm::toMat4(m_rotation);
	m_global = glm::scale(m_global, m_scale);

	m_dirty = false;

	return *this;
}

Transform& Transform::ComputeScalessGlobal()
{
	//parent stuff
	if (m_parent != entt::null) {
		//check if the parent exists, just in case
		if (ECS::GetRegistry()->valid(m_parent)) {
			m_global = ECS::GetComponent<Transform>(m_parent).GetScalessModel();
		}
		else	UnChild();

	}
	else	m_global = one;

	m_global = glm::translate(m_global, m_position);
	m_global *= glm::toMat4(m_rotation);

	m_dirty = false;

	return *this;
}

glm::mat4 Transform::GetModel()
{
	if (m_parent != entt::null)	m_dirty = true;
	if (m_dirty)				ComputeGlobal();

	return m_global;
}

glm::mat4 Transform::GetScalessModel()
{
	glm::mat4 temp = one;
	//parent stuff
	if (m_parent != entt::null) {
		//check if the parent exists, just in case
		if (ECS::GetRegistry()->valid(m_parent)) {
			temp = ECS::GetComponent<Transform>(m_parent).GetScalessModel();
		}
		else	UnChild();

	}
	
	return glm::translate(temp, m_position) * glm::toMat4(m_rotation);
}

void Transform::SetTransform(const btTransform& trans)
{
	SetPosition(trans.getOrigin());
	SetRotation(trans.getRotation());
	m_dirty = true;
}

Transform& Transform::SetPosition(const glm::vec3& pos)
{
	m_position = pos;
	m_dirty = true;

	return *this;
}

Transform& Transform::SetPosition(const btVector3& pos)
{
	m_position.x = pos.x();
	m_position.y = pos.y();
	m_position.z = pos.z();
	m_dirty = true;

	return *this;
}

glm::vec3 Transform::GetLocalPosition()
{
	return m_position;
}

glm::vec3 Transform::GetGlobalPosition()
{
	if (m_parent != entt::null)	m_dirty = true;
	if (m_dirty)		ComputeGlobal();

	return glm::vec3(m_global[3]);
}

Transform& Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
	m_dirty = true;

	return *this;
}

Transform& Transform::SetScale(float scale)
{
	m_scale.x = scale;
	m_scale.y = scale;
	m_scale.z = scale;
	m_dirty = true;

	return *this;
}

glm::vec3 Transform::GetScale()
{
	return m_scale;
}

Transform& Transform::SetRotation(const glm::mat3& rot)
{
	m_rotation = rot;
	m_dirty = true;

	return *this;
}

Transform& Transform::SetRotation(const glm::quat& rot)
{
	m_rotation = rot;
	m_dirty = true;

	return *this;
}

Transform& Transform::SetRotation(const btQuaternion& rot)
{
	m_rotation.x = rot.x();
	m_rotation.y = rot.y();
	m_rotation.z = rot.z();
	m_rotation.w = rot.w();
	m_dirty = true;

	return *this;
}

glm::quat Transform::GetLocalRotation()
{
	return m_rotation;
}

glm::quat Transform::GetGlobalRotation()
{
	if (m_parent != entt::null)	m_dirty = true;
	if (m_dirty)		ComputeGlobal();

	return glm::mat3(m_global);
}

glm::mat3 Transform::GetLocalRotationM3()
{
	return glm::toMat3(m_rotation);
}

glm::mat3 Transform::GetGlobalRotationM3()
{
	if (m_parent != entt::null)	m_dirty = true;
	if (m_dirty)				ComputeGlobal();

	return glm::mat3(m_global);
}

glm::vec3 Transform::GetForwards()
{
	if (m_parent != entt::null)	m_dirty = true;
	if (m_dirty)				ComputeGlobal();

	return glm::normalize(glm::vec3(m_global[2][0], m_global[2][1], m_global[2][2]));
}

Transform& Transform::LookAt(const glm::vec3 pos)
{
	m_rotation = glm::quatLookAt(glm::normalize(pos - GetGlobalPosition()), BLM::GLMup);
	m_dirty = true;

	return *this;
}
