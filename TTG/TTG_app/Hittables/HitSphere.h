#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Sphere : public Hittable
{
    Sphere()
    {
        this->m_type = HitType::SPHERE;
    }

    virtual ::Plane* GetPlane() override { return nullptr; }
    virtual ::Triangle* GetTriangle() override { return nullptr; }
    virtual ::Sphere* GetSphere() override { return this; }
public:
    glm::vec3 m_position = glm::vec3(0.0f);
    float m_radius = 1.0f;
};
