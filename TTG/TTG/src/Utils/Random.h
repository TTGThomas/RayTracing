#pragma once

#include <random>

#include <glm/glm.hpp>

namespace TTG
{
	class Random
	{
	public:
		static int PCG_hash(unsigned int& input)
		{
			unsigned int state = input * 74883032u + 28443256u;
			unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 27745378u;
            input = word;
			return (word >> 22u) ^ word;
		}

		static float Float(float min, float max, unsigned int& seed)
		{
			seed = PCG_hash(seed);
			//return (((float)rand() / (float)RAND_MAX)) * (max - min) + min;
			return (((float)seed / (float)UINT32_MAX)) * (max - min) + min;
		}

		static int Int(int min, int max)
		{
			return rand();
		}

		static glm::vec3 Vec3(float min, float max, unsigned int& seed)
		{
			return { Float(min, max, seed), Float(min, max, seed), Float(min, max, seed) };
		}

		static glm::vec4 Vec4(float min, float max, unsigned int& seed)
		{
			return { Vec3(min, max, seed), Float(min, max, seed) };
		}

		static glm::vec3 InUnitSphere(unsigned int& seed)
		{
			return glm::normalize(Vec3(-1.0f, 1.0f, seed));
		}




		static float Float(float min, float max)
		{
#define USEMT19937 0
#if USEMT19937
            std::uniform_real_distribution<> dist(min, max);
            return dist(m_gen);
#else
			return (((float)rand() / (float)RAND_MAX)) * (max - min) + min;
#endif
		}

		static glm::vec3 Vec3(float min, float max)
		{
			return { Float(min, max), Float(min, max), Float(min, max) };
		}

		static glm::vec4 Vec4(float min, float max)
		{
			return { Vec3(min, max), Float(min, max) };
		}

		static glm::vec3 InUnitSphere()
		{
			return glm::normalize(Vec3(-1.0f, 1.0f));
		}
    private:
        static std::random_device m_rd;
        static std::mt19937 m_gen;
	};
}
