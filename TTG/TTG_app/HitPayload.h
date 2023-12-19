#pragma once

#include <glm/glm.hpp>

enum HitType
{
	UNKNOWN, SPHERE, PLANE, TRIANGLE
};

struct HitPayload
{
	bool hasHit;
	glm::vec3 hitPos;
	glm::vec3 hitNormal;

	int hitIndex;
	float hitDistance;
	HitType hitType;
};