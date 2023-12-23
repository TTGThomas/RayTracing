#pragma once

#include "../Ray.h"

#include "../HitPayload.h"

#include "Utils/Math.h"

struct Plane;
struct Triangle;
struct Sphere;

struct Hittable
{
public:
    virtual ::Plane* GetPlane() { return nullptr; }
    virtual ::Triangle* GetTriangle() { return nullptr; }
    virtual ::Sphere* GetSphere() { return nullptr; }
public:
    HitType m_type;
    int m_matIndex = 0;
    int m_bsdfIndex = 0;
    bool m_visible = true;
};
