#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "bullet/btBulletDynamicsCommon.h"

class Transform
{
public:
	Transform();

	Transform& ChildTo(unsigned index);
	Transform& UnChild();
	Transform& ComputeGlobal();
	glm::mat4 GetModel();

	Transform& SetTransform(const btTransform& trans);

	Transform& SetPosition(const glm::vec3& pos);
	Transform& SetPosition(const btVector3& pos);
	glm::vec3 GetPosition();
	glm::vec3 GetGlobalPosition();

	Transform& SetScale(const glm::vec3& scale);
	glm::vec3 GetScale();

	Transform& SetRotation(const glm::mat3& rot);
	Transform& SetRotation(const glm::quat& rot);
	Transform& SetRotation(const btQuaternion& rot);
	glm::quat GetRotation();
	glm::mat3 GetRotationM3();

private:
	bool m_hasParent = false;
	unsigned m_parent = 0;

	bool m_dirty = true;
	glm::mat4 m_global = glm::mat4(1.f);

	glm::vec3 m_position = glm::vec3(0.f);
	glm::vec3 m_scale = glm::vec3(1.f);
	glm::quat m_rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
};