#pragma once
#include "Components/Transform.h"
#include <vector>

class CatmullFollower
{
public:
	CatmullFollower(float speed, std::vector<glm::vec3> points, int samples);

	CatmullFollower& Update(float dt);

	void SetSpeed(float speed) { m_speed = speed; }
	float GetSpeed() { return m_speed; }

	glm::vec3 GetPosition();

	glm::quat GetLookingForwards(float ahead);

private:
	glm::vec3 CalcCatmull(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);
	glm::vec3 GetPoint(float AddedDistance = 0);

	bool m_dirty = true;

	int m_samples = 8;
	int m_currentPoint = 0;
	int m_currentSample = 0;

	float m_speed;
	float m_distance = 0;
	float m_totalDistance = 0;

	struct Point
	{
		float t;
		float accumDistance;
		glm::vec3 pos;
	};

	std::vector<std::vector<Point>> m_points = {};

	glm::vec3 m_position;
};

