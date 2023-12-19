#include "TestBsdf.h"

void TestBsdf::processHit(BsdfPayload payload, Ray* ray)
{
	HitPayload hit = payload.payload;
	Material mat = payload.mat;
	ray->Origin = hit.hitPos + (hit.hitNormal * 0.001f);
	ray->Direction = glm::vec3(0.0f, 1.0f, 0.0f);
}
