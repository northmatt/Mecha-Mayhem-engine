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

void Transform::ComputeGlobal()
{
	//parent stuff
	if (m_hasParent) {
		//check if the parent exists, just in case
		if (ECS::GetRegistry()->valid(m_parent)) {
			m_global = ECS::GetComponent<Transform>(m_parent).GetModel();
		}
		else	UnChild();

	}
	else	m_global = one;

	m_global = glm::translate(m_global, m_position);
	m_global *= glm::toMat4(m_rotation);
	m_global = glm::scale(m_global, m_scale);
}

glm::mat4 Transform::GetModel()
{
	if (m_dirty)	ComputeGlobal();

	return m_global;
}

Transform& Transform::SetPosition(const glm::vec3& pos)
{
	m_position = pos;
	m_dirty = true;

	return *this;
}

glm::vec3 Transform::GetPosition()
{
	return m_position;
}

Transform& Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
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

glm::quat Transform::GetRotation()
{
	return m_rotation;
}

glm::mat3 Transform::GetRotationM3()
{
	return glm::toMat3(m_rotation);
}
