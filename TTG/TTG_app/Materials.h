#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec4 albedo = glm::vec4(0.0f);
	float roughness = 1.0f;
	float metalic = 0.0f;

	glm::vec3 emissionColor = glm::vec4(0.0f);
	float emissionPower = 0.0f;

	glm::vec3 getEmission()
	{
		return emissionColor * emissionPower;
	}
};
