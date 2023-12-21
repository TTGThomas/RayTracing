#include "TestBsdf.h"

void TestBsdf::ProcessHit(BsdfPayload& payload, Ray* ray)
{
	HitPayload& hit = payload.m_payload;
	Material& mat = payload.m_mat;
	ray->m_origin = hit.m_hitPos + (hit.m_hitNormal * 0.001f);
	ray->m_direction = glm::vec3(0.0f, 1.0f, 0.0f);
}
