#pragma once

#include <glm/glm.hpp>
#include "ImGuiOpenGL.h"
#include "../HitPayload.h"
#include "../Materials.h"
#include "../Ray.h"

struct BsdfPayload
{
	HitPayload m_payload;
	Material m_mat;
};

class Bsdf
{
public:
	virtual void ProcessHit(BsdfPayload& payload, Ray* ray) {}
	virtual void AdvanceSettings() {}
};