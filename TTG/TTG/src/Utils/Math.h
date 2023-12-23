#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>

namespace TTG
{
	namespace Math
	{
		inline float Dot(const glm::vec3& a, const glm::vec3& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		inline float Sqrt(const float& in)
		{
			return std::sqrt(in);
			//return glm::sqrt(in);
		}

		inline float Length(const glm::vec3& in)
		{
			return sqrt(dot(in, in));
		}

		inline glm::vec3 Normalize(const glm::vec3& in)
		{
			return in / length(in);
		}

		inline glm::vec3 Cross(const glm::vec3& x, const glm::vec3& y)
		{
			return glm::vec3(
				x.y * y.z - y.y * x.z,
				x.z * y.x - y.z * x.x,
				x.x * y.y - y.x * x.y);
		}
	}
}