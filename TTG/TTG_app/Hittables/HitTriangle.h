#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Triangle : public Hittable
{
    Triangle()
    {
        this->m_type = HitType::TRIANGLE;
    }
    
    glm::vec3 m_posA = glm::vec3(0.0f);
    glm::vec3 m_posB = glm::vec3(0.0f);
    glm::vec3 m_posC = glm::vec3(0.0f);

    glm::vec3 m_na = glm::vec3(0.0f);
    glm::vec3 m_nb = glm::vec3(0.0f);
    glm::vec3 m_nc = glm::vec3(0.0f);
    
    virtual float Hit(const Ray& ray) override
    {
        return -1.0f;
    }
};
