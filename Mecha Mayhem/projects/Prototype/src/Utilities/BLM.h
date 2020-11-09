#pragma once
#include <bullet/LinearMath/btTransform.h>
#include <glm/common.hpp>

//class to convert between BT and GLM, name not on purpose
class BLM abstract
{
public:
	static glm::vec3 BTtoGLM(btVector3 vec);
	static btVector3 GLMtoBT(glm::vec3 vec);
	static glm::quat BTtoGLM(btQuaternion quat);
	static btQuaternion GLMtoBT(glm::quat quat);
};

inline glm::vec3 BLM::BTtoGLM(btVector3 vec)
{
	return glm::vec3(vec.x(), vec.y(), vec.z());
}

inline btVector3 BLM::GLMtoBT(glm::vec3 vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

inline glm::quat BLM::BTtoGLM(btQuaternion quat)
{
	return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
}

inline btQuaternion BLM::GLMtoBT(glm::quat quat)
{
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}
