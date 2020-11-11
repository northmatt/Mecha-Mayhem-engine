#include "Transform.h"
#include "ECS.h"

glm::mat4 one = glm::mat4(1.f);

Transform::Transform()
{

}

Transform& Transform::ChildTo(unsigned index)
{
	//check if it exists
	if (ECS::GetRegistry()->valid(index)) {
		m_hasParent = true;
		m_parent = index;
	}

	return *this;
}

Transform& Transform::UnChild()
{
	m_hasParent = false;
	m_parent = 0;

	return *this;
}

Transform& Transform::ComputeGlobal()
{
	//parent stuff
	if (m_hasParent) {
		//check if the parent exists, just in case
		if (ECS::GetRegistry()->valid(m_parent)) {
			m_global = ECS::GetComponent<Transform>(m_parent).GetModel();
			//m_global[3][2] = -m_global[3][2];
		}
		else	UnChild();

	}
	else	m_global = one;

	m_global = glm::translate(m_global, m_position);
	m_global *= glm::toMat4(m_rotation);
	m_global = glm::scale(m_global, m_scale);

	//m_global[3][2] = -m_global[3][2];

	m_dirty = false;

	return *this;
}

glm::mat4 Transform::GetModel()
{
	if (m_hasParent)	m_dirty = true;
	if (m_dirty)		ComputeGlobal();

	return m_global;
}

Transform& Transform::SetTransform(const btTransform& trans)
{
	SetPosition(trans.getOrigin());
	SetRotation(trans.getRotation());

	m_dirty = true;

	return *this;
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

	return *this;
}

glm::vec3 Transform::GetPosition()
{
	return m_position;
}

glm::vec3 Transform::GetGlobalPosition()
{
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

	return *this;
}

glm::quat Transform::GetRotation()
{
	return m_rotation;
}

glm::quat Transform::GetGlobalRotation()
{
	return glm::mat3(m_global);
}

glm::mat3 Transform::GetRotationM3()
{
	return glm::toMat3(m_rotation);
}

glm::mat3 Transform::GetGlobalRotationM3()
{
	return glm::mat3(m_global);
}

glm::vec3 Transform::GetForwards()
{
	return glm::rotate(GetGlobalRotation(), glm::vec3(0, 0, 1));
}
