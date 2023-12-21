#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Triangle : public Hittable
{
    Triangle()
    {
        this->type = HIT_TYPES::TRIANGLE;
    }
    
    glm::vec3 PosA = glm::vec3(0.0f);
    glm::vec3 PosB = glm::vec3(0.0f);
    glm::vec3 PosC = glm::vec3(0.0f);

    glm::vec3 na = glm::vec3(0.0f);
    glm::vec3 nb = glm::vec3(0.0f);
    glm::vec3 nc = glm::vec3(0.0f);
    
    virtual float Hit(const Ray& ray) override
    {
        return -1.0f;
    }
};
