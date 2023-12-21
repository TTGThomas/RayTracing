#pragma once

#include <glm/glm.hpp>

enum HitType
{
	UNKNOWN, SPHERE, PLANE, TRIANGLE
};

struct HitPayload
{
	bool m_hasHit;
	glm::vec3 m_hitPos;
	glm::vec3 m_hitNormal;

	int m_hitIndex;
	float m_hitDistance;
	HitType m_hitType;
};