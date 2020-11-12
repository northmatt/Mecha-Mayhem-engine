#pragma once
#include <bullet/LinearMath/btTransform.h>
#include <glm/common.hpp>

//class to convert between BT and GLM, name not on purpose
class BLM abstract
{
public:
	static glm::vec3 BTtoGLM(const btVector3& vec);
	static btVector3 GLMtoBT(const glm::vec3& vec);
	static glm::quat BTtoGLM(const btQuaternion& quat);
	static btQuaternion GLMtoBT(const glm::quat& quat);
};

inline glm::vec3 BLM::BTtoGLM(const btVector3& vec)
{
	return glm::vec3(vec.x(), vec.y(), vec.z());
}

inline btVector3 BLM::GLMtoBT(const glm::vec3& vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

inline glm::quat BLM::BTtoGLM(const btQuaternion& quat)
{
	return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
}

inline btQuaternion BLM::GLMtoBT(const glm::quat& quat)
{
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}
