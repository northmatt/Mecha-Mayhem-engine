#include "Catmull.h"

CatmullFollower::CatmullFollower(float speed, std::vector<glm::vec3> points, int samples)
	:m_speed(speed)
{
	if (samples < 0)
		m_samples = 0;
	else if (samples > 16)
		m_samples = 16;
	else m_samples = samples;

	if (points.size() < 4)
		m_samples = 0;

	if (m_samples == 0) {
		m_position = BLM::GLMzero;
		return;
	}

	m_position = points[0];


	//calculating things
	m_totalDistance = 0;
	float oneOverSamples = 1.f / samples;
	int p0, p1, p2, p3;
	for (int i(0); i < points.size(); ++i) {

		p1 = i;
		p0 = p1 - 1;
		if (p0 < 0)
			p0 = points.size() - 1;
		p2 = p1 + 1;
		if (p2 >= points.size())
			p2 = 0;
		p3 = p2 + 1;
		if (p3 >= points.size())
			p3 = 0;

		std::vector<Point> currSamples;

		currSamples.push_back({ 0, m_totalDistance, points[i] });

		for (int j(1); j <= m_samples; ++j) {
			float t = j * oneOverSamples;
			glm::vec3 pos = CalcCatmull(points[p0], points[p1], points[p2], points[p3], t);
			currSamples.push_back({ t, m_totalDistance += glm::length(pos - currSamples[j - 1].pos), pos });
		}
		m_points.push_back(currSamples);
	}
}

CatmullFollower& CatmullFollower::Update(float dt)
{
	if (m_samples == 0)	return *this;

	m_distance += m_speed * dt;
	while (m_distance >= m_totalDistance) {
		m_distance -= m_totalDistance;
		m_currentPoint = 0;
		m_currentSample = 0;
	}
	m_dirty = true;
	return *this;
}

glm::vec3 CatmullFollower::GetPosition()
{
	if (m_samples == 0)	return m_position;

	if (m_dirty) {
		m_dirty = false;
		return m_position = GetPoint();
	}
	return m_position;
}

glm::quat CatmullFollower::GetLookingForwards(float ahead)
{
	if (m_samples == 0)	return BLM::GLMQuat;

	return glm::quatLookAt(glm::normalize(GetPoint(ahead) - GetPosition()), BLM::GLMup);
}

glm::vec3 CatmullFollower::CalcCatmull(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
	return 0.5f * (2.f * p1 + t * (-p0 + p2)
		+ t * t * (2.f * p0 - 5.f * p1 + 4.f * p2 - p3)
		+ t * t * t * (-p0 + 3.f * p1 - 3.f * p2 + p3));
}

glm::vec3 CatmullFollower::GetPoint(float AddedDistance)
{
	int currP = m_currentPoint;
	int currS = m_currentSample;
	float distance = m_distance + AddedDistance;

	while (m_points[currP][currS + 1].accumDistance < distance) {
		if (++currS >= m_samples) {
			currS = 0;
			if (++currP >= m_points.size()) {
				currP = 0;
				distance -= m_totalDistance;
			}
		}
	}

	float insideT = (distance - m_points[currP][currS].accumDistance) /
		(m_points[currP][currS + 1].accumDistance - m_points[currP][currS].accumDistance);

	float t = (1 - insideT) * m_points[currP][currS].t + insideT * m_points[currP][currS + 1].t;

	int p0 = currP - 1;
	if (p0 < 0)        p0 = m_points.size() - 1;
	int p1 = currP;
	int p2 = p1 + 1;
	if (p2 >= m_points.size())   p2 = 0;
	int p3 = p2 + 1;
	if (p3 >= m_points.size())   p3 = 0;

	if (AddedDistance == 0) {
		m_currentPoint = currP;
		m_currentSample = currS;
		m_distance = distance;
	}

	return CalcCatmull(m_points[p0][0].pos, m_points[p1][0].pos, m_points[p2][0].pos, m_points[p3][0].pos, t);
}
