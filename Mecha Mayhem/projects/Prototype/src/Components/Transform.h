#pragma once

#include "bullet/btBulletDynamicsCommon.h"
#include "Utilities/BLM.h"
#include <entt/entt.hpp>

class Transform
{
public:
	Transform();
	Transform(const glm::vec3& pos);
	Transform(const glm::vec3& pos, const glm::quat& rot);
	Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);

	Transform& ChildTo(entt::entity index);
	bool HasParent() { return m_parent != entt::null; }
	Transform& UnChild(bool conservePosition = true);

	Transform& SetUsingParentScale(bool yes);
	bool GetUsingParentScale() { return m_usingParentScale; }
	Transform& UseAsParent(Transform other);
	Transform& UseAsParent(const glm::mat4& model);

	Transform& ComputeGlobal();
	Transform& ComputeScalessGlobal();
	glm::mat4 GetModel();
	glm::mat4 GetScalessModel();

	//use only in scene.cpp
	void SetTransform(const btTransform& trans);

	Transform& SetPosition(const glm::vec3& pos);
	Transform& SetPosition(const btVector3& pos);
	glm::vec3 GetLocalPosition();
	glm::vec3 GetGlobalPosition();

	Transform& SetScale(const glm::vec3& scale);
	Transform& SetScale(float scale);
	glm::vec3 GetScale();

	Transform& SetRotation(const glm::mat3& rot);
	Transform& SetRotation(const glm::quat& rot);
	Transform& SetRotation(const btQuaternion& rot);

	glm::quat GetLocalRotation();
	//doesn't always work
	glm::quat GetGlobalRotation();

	glm::mat3 GetLocalRotationM3();
	//doesn't always work
	glm::mat3 GetGlobalRotationM3();

	glm::vec3 GetForwards();
	Transform& LookAt(const glm::vec3 pos);

private:
	entt::entity m_parent = entt::null;
	bool m_usingParentScale = true;

	bool m_dirty = true;
	glm::mat4 m_global = glm::mat4(1.f);

	glm::vec3 m_position = glm::vec3(0.f);
	glm::vec3 m_scale = glm::vec3(1.f);
	glm::quat m_rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
};