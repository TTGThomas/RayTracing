#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../Materials/Bsdf.h"

#include "Materials.h"

#include "HittableBase.h"

#include "HitTriangle.h"
#include "HitSphere.h"
#include "HitPlane.h"

struct Scene
{
	std::vector<Hittable> objects;
	std::vector<Material> mat;
	std::vector<std::unique_ptr<Bsdf>> bsdf;
};
