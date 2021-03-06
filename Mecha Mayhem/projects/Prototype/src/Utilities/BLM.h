#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
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

	static const btVector3 BTzero;
	static const glm::vec3 GLMup;
	static const glm::vec3 GLMzero;
	static const glm::quat GLMQuat;
	static const glm::mat4 GLMMat;
	static constexpr float pi = glm::pi<float>();
};

inline const btVector3 BLM::BTzero = btVector3(0, 0, 0);
inline const glm::vec3 BLM::GLMup = glm::vec3(0, 1, 0);
inline const glm::vec3 BLM::GLMzero = glm::vec3(0, 0, 0);
inline const glm::quat BLM::GLMQuat = glm::quat(1.f, 0, 0, 0);
inline const glm::mat4 BLM::GLMMat = glm::mat4(1.f);

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
