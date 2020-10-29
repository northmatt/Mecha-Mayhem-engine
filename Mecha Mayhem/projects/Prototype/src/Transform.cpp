#include "Transform.h"

Transform::Transform()
{

}

glm::mat4 Transform::GetModel()
{
	glm::mat4 Model = glm::translate(glm::mat4(1.f), glm::vec3(-m_position.x, m_position.y, m_position.z));
	Model *= glm::toMat4(m_rotation);
	Model = glm::scale(Model, m_scale);

	return Model;
}

void Transform::SetPosition(const glm::vec3& pos)
{
	m_position = pos;
}

glm::vec3 Transform::GetPosition()
{
	return m_position;
}

void Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
}

glm::vec3 Transform::GetScale()
{
	return m_scale;
}

void Transform::SetRotation(const glm::mat3& rot)
{
	m_rotation = rot;
}

void Transform::SetRotation(const glm::quat& rot)
{
	m_rotation = rot;
}

glm::quat Transform::GetRotation()
{
	return m_rotation;
}

glm::mat3 Transform::GetRotationM3()
{
	return glm::toMat3(m_rotation);
}
