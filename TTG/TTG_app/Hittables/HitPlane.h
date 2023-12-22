#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Plane : public Hittable
{
    Plane()
    {
        this->m_type = HitType::PLANE;
    }
    
    virtual float Hit(const Ray& ray) override
    {
        if (!m_visible)
            return -1.0f;

        float denomanator = TTG::Math::Dot(m_normal, ray.m_direction);
        if (denomanator == 0)
            return -1.0f;

        float numerator = TTG::Math::Dot(m_normal, m_position * glm::vec3(1.0f, -1.0f, 1.0f)) - TTG::Math::Dot(m_normal, ray.m_origin);

        return numerator / denomanator;
    }
public:
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_normal = glm::vec3(0.0f, 1.0f, 0.0f);
};
