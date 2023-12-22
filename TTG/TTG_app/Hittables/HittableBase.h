#pragma once

#include "../Ray.h"

#include "../HitPayload.h"

#include "Utils/Math.h"

struct Hittable
{   
    virtual float Hit(const Ray& ray) { return -1.0f; }
public:
    HitType m_type;
    int m_matIndex = 0;
    int m_bsdfIndex = 0;
    bool m_visible = true;
};
