#pragma once

#include "Bsdf.h"

#include "Utils/Math.h"
#include "Utils/Random.h"

class GlassBsdf : public Bsdf
{
public:
	virtual void processHit(BsdfPayload payload, Ray* ray) override;
	virtual void advanceSetting() override;
private:
	glm::vec3 refract(Ray ray, HitPayload payload, float refractRatio);
	float reflectance(float cosine, float refractionRatio);
private:
	float reflectIndex = 1.2f;
};