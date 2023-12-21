#include "GlassBsdf.h"

void GlassBsdf::ProcessHit(BsdfPayload& payload, Ray* ray)
{
	HitPayload& hit = payload.m_payload;
    
    ray->m_direction = TTG::Math::Normalize(ray->m_direction);
    
    bool frontFace;
	if (TTG::Math::Dot(ray->m_direction, hit.m_hitNormal) > 0.0f)
	{
        ray->m_origin = hit.m_hitPos + (hit.m_hitNormal * 0.001f);
        hit.m_hitNormal = -hit.m_hitNormal;
        frontFace = false;
	}
	else
	{
        ray->m_origin = hit.m_hitPos - (hit.m_hitNormal * 0.001f);
        frontFace = true;
	}

	float refractRatio = frontFace ? (1.0f / m_reflectIndex) : m_reflectIndex;
	
	ray->m_direction = Refract(*ray, hit, refractRatio);

	float cos_theta = std::fmin(TTG::Math::Dot(-ray->m_direction, hit.m_hitNormal), 1.0f);
	float sin_theta = TTG::Math::Sqrt(1.0f - (cos_theta * cos_theta));
	if (refractRatio * sin_theta > 1.0f || Reflectance(cos_theta, refractRatio) > TTG::Random::Float(0.0f, 1.0f))
		ray->m_direction = glm::reflect(ray->m_direction, hit.m_hitNormal);
	else
		ray->m_direction = Refract(*ray, hit, refractRatio);
}

void GlassBsdf::AdvanceSettings()
{
	ImGui::DragFloat("reflection index", &m_reflectIndex, 0.01f, 1.0f, 5.0f);
}

glm::vec3 GlassBsdf::Refract(Ray& ray, HitPayload& payload, float refractRatio)
{
	float cos_theta = std::fmin(TTG::Math::Dot(-ray.m_direction, payload.m_hitNormal), 1.0f);
	glm::vec3 perp = refractRatio * (ray.m_direction + cos_theta * payload.m_hitNormal);
	glm::vec3 para = -TTG::Math::Sqrt(std::fabs(1.0f - TTG::Math::Dot(perp, perp))) * payload.m_hitNormal;
	return perp + para;
}

float GlassBsdf::Reflectance(float cosine, float refractionRatio)
{
	float r0 = (1.0f - refractionRatio) / (1.0f + refractionRatio);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * (float)std::pow((1.0f - cosine), 5.0f);
}
