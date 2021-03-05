#pragma once

class Time abstract
{
public:
	static float dt;

	static void Update(float currClock) {
		dt = currClock - m_lastClock;
		if (dt > tooLong)
			dt = tooLong;
		m_lastClock = glfwGetTime();
	}

private:
	static float m_lastClock;
	static const float tooLong;
};
//Delta Time
inline float Time::dt = 0;
inline float Time::m_lastClock = 0;
inline const float Time::tooLong = 0.5f;