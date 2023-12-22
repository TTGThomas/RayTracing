#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Materials/Bsdf.h"

#include "Materials.h"

#include "Hittables/HittableBase.h"

#include "Hittables/HitTriangle.h"
#include "Hittables/HitSphere.h"
#include "Hittables/HitPlane.h"

#include <memory>

struct Scene
{
	std::vector<std::unique_ptr<Hittable>> objects;
	std::vector<Material> mat;
	std::vector<std::unique_ptr<Bsdf>> bsdf;
};
