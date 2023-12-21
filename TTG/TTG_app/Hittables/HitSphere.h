#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Sphere : public Hittable
{
    Sphere()
    {
        this->m_type = HitType::SPHERE;
    }

    glm::vec3 m_position = glm::vec3(0.0f);
    float m_radius = 1.0f;
    
    virtual float Hit(const Ray& ray) override
    {
        return -1.0f;
    }
};
