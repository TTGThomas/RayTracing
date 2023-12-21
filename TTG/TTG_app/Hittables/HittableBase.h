#pragma once

#include "../Ray.h"

enum class HIT_TYPES
{
    UNKNOWN, SPHERE, PLANE, TRIANGLE
};

struct Hittable
{
    HIT_TYPES type;
    int matIndex = 0;
    int bsdfIndex = 0;
    bool visible = true;
    
    virtual float Hit(const Ray& ray) { return -1.0f; }
};
