#include "BasicBsdf.h"

void BasicBsdf::processHit(BsdfPayload payload, Ray* ray)
{
	HitPayload hit = payload.payload;
	Material mat = payload.mat;
	glm::vec3 preCalculateRand;
    preCalculateRand = TTG::Random::inUnitSphere();


	bool fontFace = TTG::Math::dot(ray->Direction, hit.hitNormal) < 0.0f;

	if (!fontFace)
		hit.hitNormal = -hit.hitNormal;

	ray->Origin = hit.hitPos + (hit.hitNormal * 0.001f);

	glm::vec3 reflectDir = TTG::Math::normalize(glm::reflect(ray->Direction, hit.hitNormal));

	glm::vec3 randomDir = TTG::Math::normalize(hit.hitNormal + preCalculateRand);
	
	ray->Direction = (mat.roughness * randomDir) + ((1.0f - mat.roughness) * reflectDir);
}
