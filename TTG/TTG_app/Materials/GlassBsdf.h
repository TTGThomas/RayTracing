#pragma once

#include "Bsdf.h"

#include "Utils/Math.h"
#include "Utils/Random.h"

class GlassBsdf : public Bsdf// actually a BRDF, but who cares.
{
public:
	virtual void ProcessHit(BsdfPayload& payload, Ray* ray) override;
	virtual void AdvanceSettings() override;
private:
	glm::vec3 Refract(Ray& ray, HitPayload& payload, float refractRatio);
	float Reflectance(float cosine, float refractionRatio);
private:
	float m_reflectIndex = 1.2f;
};