#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec4 m_albedo = glm::vec4(0.0f);
	float m_roughness = 1.0f;
	float m_metalic = 0.0f;

	glm::vec3 m_emissionColor = glm::vec4(0.0f);
	float m_emissionPower = 0.0f;

	glm::vec3 GetEmission()
	{
		return m_emissionColor * m_emissionPower;
	}
};
