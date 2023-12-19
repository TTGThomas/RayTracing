#pragma once

#include <glm/glm.hpp>
#include "ImGuiOpenGL.h"
#include "../HitPayload.h"
#include "../Materials.h"
#include "../Ray.h"

struct BsdfPayload
{
	HitPayload payload;

	bool preCalculate = false;
	glm::vec3 preCalculateRand = glm::vec3(0.0f);

	Material mat;
};

class Bsdf
{
public:
	virtual void processHit(BsdfPayload payload, Ray* ray) {}
	virtual void advanceSetting() {}
};