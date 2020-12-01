#pragma once

class Time abstract
{
public:
	static float dt;

	static void Update(float currClock) {
		dt = currClock - m_lastClock;
		m_lastClock = currClock;
	}
private:
	static float m_lastClock;
};
//Delta Time
inline float Time::dt = 0;
inline float Time::m_lastClock = 0;