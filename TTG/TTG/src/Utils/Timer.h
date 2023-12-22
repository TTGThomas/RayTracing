#pragma once

#include <chrono>

namespace TTG
{
	class Timer
	{
	public:
		Timer() = default;

		~Timer() = default;

		void Start()
		{
			m_started = true;
			m_start = std::chrono::high_resolution_clock::now();
		}

		void Stop()
		{
			if (m_started)
			{
				m_started = false;
				m_end = std::chrono::high_resolution_clock::now();
				m_duration = m_end - m_start;
			}
		}

		float Ms()
		{
			float ms = m_duration.count() * 1000.0f;
			return ms;
		}

		float Seconds()
		{
			float ms = m_duration.count();
			return ms;
		}
	private:
		std::chrono::steady_clock::time_point m_start{}, m_end{};
		std::chrono::duration<float> m_duration{};
		bool m_started = false;
	};
}