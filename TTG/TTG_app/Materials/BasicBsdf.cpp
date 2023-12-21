#include "BasicBsdf.h"

void BasicBsdf::ProcessHit(BsdfPayload& payload, Ray* ray)
{
	HitPayload hit = payload.m_payload;
	Material mat = payload.m_mat;

	bool fontFace = TTG::Math::Dot(ray->m_direction, hit.m_hitNormal) < 0.0f;

	if (!fontFace)
		hit.m_hitNormal = -hit.m_hitNormal;

	ray->m_origin = hit.m_hitPos + (hit.m_hitNormal * 0.001f);

	glm::vec3 reflectDir = TTG::Math::Normalize(glm::reflect(ray->m_direction, hit.m_hitNormal));

	glm::vec3 randomDir = TTG::Math::Normalize(hit.m_hitNormal + TTG::Random::InUnitSphere());
	
	ray->m_direction = (mat.m_roughness * randomDir) + ((1.0f - mat.m_roughness) * reflectDir);
}
