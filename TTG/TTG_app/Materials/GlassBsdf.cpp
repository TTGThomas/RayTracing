#include "GlassBsdf.h"

void GlassBsdf::processHit(BsdfPayload payload, Ray* ray)
{
	HitPayload hit = payload.payload;
    
    ray->Direction = TTG::Math::normalize(ray->Direction);
    
    bool frontFace;
	if (TTG::Math::dot(ray->Direction, hit.hitNormal) > 0.0f)
	{
        ray->Origin = hit.hitPos + (hit.hitNormal * 0.001f);
        hit.hitNormal = -hit.hitNormal;
        frontFace = false;
	}
	else
	{
        ray->Origin = hit.hitPos - (hit.hitNormal * 0.001f);
        frontFace = true;
	}

	float refractRatio = frontFace ? (1.0f / reflectIndex) : reflectIndex;
	
	ray->Direction = refract(*ray, hit, refractRatio);

	//float cos_theta = std::fmin(TTG::Math::dot(-ray->Direction, hit.hitNormal), 1.0f);
	//float sin_theta = TTG::Math::sqrt(1.0f - (cos_theta * cos_theta));
	//if (refractRatio * sin_theta > 1.0f || reflectance(cos_theta, refractRatio) > TTG::Random::Float(0.0f, 1.0f))
	//	ray->Direction = glm::reflect(ray->Direction, hit.hitNormal);
	//else
	//	ray->Direction = refract(*ray, hit, refractRatio);
}

void GlassBsdf::advanceSetting()
{
	ImGui::DragFloat("reflect index", &reflectIndex, 0.01f, 1.0f, 5.0f);
}

glm::vec3 GlassBsdf::refract(Ray ray, HitPayload payload, float refractRatio)
{
	float cos_theta = std::fmin(TTG::Math::dot(-ray.Direction, payload.hitNormal), 1.0f);
	glm::vec3 perp = refractRatio * (ray.Direction + cos_theta * payload.hitNormal);
	glm::vec3 para = -TTG::Math::sqrt(std::fabs(1.0f - TTG::Math::dot(perp, perp))) * payload.hitNormal;
	return perp + para;
}

float GlassBsdf::reflectance(float cosine, float refractionRatio)
{
	float r0 = (1.0f - refractionRatio) / (1.0f + refractionRatio);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * (float)std::pow((1.0f - cosine), 5.0f);
}
