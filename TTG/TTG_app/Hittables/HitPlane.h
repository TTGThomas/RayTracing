#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Plane : public Hittable
{
    Plane()
    {
        this->m_type = HitType::PLANE;
    }

    virtual ::Plane* GetPlane() override { return this; }
    virtual ::Triangle* GetTriangle() override { return nullptr; }
    virtual ::Sphere* GetSphere() override { return nullptr; }
public:
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_normal = glm::vec3(0.0f, 1.0f, 0.0f);
};
