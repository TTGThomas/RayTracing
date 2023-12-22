#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Triangle : public Hittable
{
    Triangle()
    {
        this->m_type = HitType::TRIANGLE;
    }

    virtual float Hit(const Ray& ray) override
    {
		if (!m_visible)
			return -1.0f;

		glm::vec3 a = m_posA;
		a.y = -a.y;

		glm::vec3 b = m_posB;
		b.y = -b.y;

		glm::vec3 c = m_posC;
		c.y = -c.y;

		glm::vec3 edgeAB = b - a;
		glm::vec3 edgeAC = c - a;
		glm::vec3 normalVector = TTG::Math::Cross(edgeAB, edgeAC);
		glm::vec3 ao = ray.m_origin - a;
		glm::vec3 dao = TTG::Math::Cross(ao, ray.m_direction);

		float determinant = -TTG::Math::Dot(ray.m_direction, normalVector);
		float invDet = 1 / determinant;

		float dst = TTG::Math::Dot(ao, normalVector) * invDet;
		m_u = TTG::Math::Dot(edgeAC, dao) * invDet;
		m_v = -TTG::Math::Dot(edgeAB, dao) * invDet;
		m_w = 1 - m_u - m_v;

		bool hasHit = determinant >= 0.0001f && dst >= 0 && m_u >= 0 && m_v >= 0 && m_w >= 0;

		if (!hasHit)
			return -1.0f;
		return dst;
    }
public:
	glm::vec3 m_posA = glm::vec3(0.0f);
	glm::vec3 m_posB = glm::vec3(0.0f);
	glm::vec3 m_posC = glm::vec3(0.0f);

	glm::vec3 m_na = glm::vec3(0.0f);
	glm::vec3 m_nb = glm::vec3(0.0f);
	glm::vec3 m_nc = glm::vec3(0.0f);

	float m_u = 0.0f, m_v = 0.0f, m_w = 0.0f;
};
