#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>

namespace TTG
{
	namespace Math
	{
		inline float dot(glm::vec3 a, glm::vec3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		inline float sqrt(float in)
		{
			return glm::sqrt(in);
			//return glm::fastSqrt<float>(in);
			//float xhalf = 0.5f * in;
			//int i = *(int*)&in;
			//i = 0x5f375a86 - (i >> 1);
			//in = *(float*)&i;
			//in = in * (1.5f - xhalf * in * in);
			//return 1 / in;
			//long i;
			//float x, y;
			//const float f = 1.5f;
			//
			//x = in * 0.5f;
			//y = in;
			//i = *(long*)&y;
			//i = 0x5f3759df - (i >> 1);
			//y = *(float*)&i;
			//y = y * (f - (x * y * y));
			//y = y * (f - (x * y * y));
			//return in * y;
		}

		inline float length(glm::vec3 in)
		{
			return sqrt(dot(in, in));
		}

		inline glm::vec3 normalize(glm::vec3 in)
		{
			return in / length(in);
		}

		inline glm::vec3 cross(glm::vec3 x, glm::vec3 y)
		{
			return glm::vec3(
				x.y * y.z - y.y * x.z,
				x.z * y.x - y.z * x.x,
				x.x * y.y - y.x * x.y);
		}
	}
}