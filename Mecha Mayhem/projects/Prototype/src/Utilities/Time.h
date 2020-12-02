#pragma once

class Time abstract
{
public:
	static float dt;

	static void Update(float currClock) {
		dt = currClock - m_lastClock;
		if (dt > OneOverDt)
			dt = OneOverDt;
		m_lastClock = currClock;
	}

private:
	static float m_lastClock;
	static const float OneOverDt;
};
//Delta Time
inline float Time::dt = 0;
inline float Time::m_lastClock = 0;
inline const float Time::OneOverDt = 1 / 60.f;