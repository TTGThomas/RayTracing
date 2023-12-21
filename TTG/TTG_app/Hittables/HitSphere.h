#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Sphere : public Hittable
{
    Sphere()
    {
        this->type = HIT_TYPES::SPHERE;
    }

    glm::vec3 Position = glm::vec3(0.0f);
    float radius = 1.0f;
    
    virtual float Hit(const Ray& ray) override
    {
        return -1.0f;
    }
};
