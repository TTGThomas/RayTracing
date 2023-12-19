#pragma once

#include <chrono>

namespace TTG
{
	class Timer
	{
	public:
		Timer()
		{
		}

		~Timer() = default;

		void start()
		{
			m_start = std::chrono::high_resolution_clock::now();
		}

		void stop()
		{
			m_end = std::chrono::high_resolution_clock::now();
			m_duration = m_end - m_start;
		}

		float ms()
		{
			float ms = m_duration.count() * 1000.0f;
			return ms;
		}

		float seconds()
		{
			float ms = m_duration.count();
			return ms;
		}
	private:
		std::chrono::steady_clock::time_point m_start, m_end;
		std::chrono::duration<float> m_duration;
	};
}