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
	static double m_lastClock;
	static const double tooLong;
};
//Delta Time
inline float Time::dt = 0;
inline double Time::m_lastClock = 0;
inline const double Time::tooLong = 0.5f;