#pragma once

#include <chrono>

class Timer
{
public:

	Timer() { Reset(); }

	void Reset()
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	float GetElapsedSeconds()
	{
		return GetElapsedMilliseconds() * 0.001f;
	}

	float GetElapsedMilliseconds()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start).count() * 0.001f * 0.001f;
	}

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

};