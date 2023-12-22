#pragma once

#include "HittableBase.h"

#include <glm/glm.hpp>

struct Sphere : public Hittable
{
    Sphere()
    {
        this->m_type = HitType::SPHERE;
    }

    virtual float Hit(const Ray& ray) override
    {
        if (!m_visible)
            return -1.0f;

        glm::vec3 sphereOrigin;
        sphereOrigin = m_position;
        sphereOrigin.y = 0.0f - sphereOrigin.y;

        float radius = m_radius;

        glm::vec3 origin = ray.m_origin - sphereOrigin;

        // origin formula = (x)^2 + (y)^2 = r^2

        // a = ray origin
        // b = ray direction
        // r = radius
        // t = hit distance
        // (bx^2 + by^2 + bz^2)t^2 + (axbx + ayby + azbz)2t + (ax^2 + ay^2 + az^2 - r^2) = 0;

        //float a = 1.0f;
        float a = TTG::Math::Dot(ray.m_direction, ray.m_direction);
        float b = TTG::Math::Dot(origin, ray.m_direction);// b / 2
        float c = TTG::Math::Dot(origin, origin) - radius * radius;

        // Quad = b^2 - 4ac

        float discriminant = (b * b) - (a * c);
        // completes the quadratic equation
        float closestT = (-b - TTG::Math::Sqrt(discriminant)) / (a);
        return closestT;
    }
public:
    glm::vec3 m_position = glm::vec3(0.0f);
    float m_radius = 1.0f;
};
