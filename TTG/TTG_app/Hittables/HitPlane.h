#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Plane : public Hittable
{
    Plane()
    {
        this->type = HIT_TYPES::PLANE;
    }

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
    
    virtual float Hit(const Ray& ray) override
    {
        return -1.0f;
    }
};