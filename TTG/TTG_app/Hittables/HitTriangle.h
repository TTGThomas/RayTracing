#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Triangle : public Hittable
{
    Triangle()
    {
        this->m_type = HitType::TRIANGLE;
    }

	virtual ::Plane* GetPlane() override { return nullptr; }
	virtual ::Triangle* GetTriangle() override { return this; }
	virtual ::Sphere* GetSphere() override { return nullptr; }
public:
	glm::vec3 m_posA = glm::vec3(0.0f);
	glm::vec3 m_posB = glm::vec3(0.0f);
	glm::vec3 m_posC = glm::vec3(0.0f);

	glm::vec3 m_na = glm::vec3(0.0f);
	glm::vec3 m_nb = glm::vec3(0.0f);
	glm::vec3 m_nc = glm::vec3(0.0f);

	float m_u = 0.0f, m_v = 0.0f, m_w = 0.0f;
};
